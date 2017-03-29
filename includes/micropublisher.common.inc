<?php

/**
 * @file
 * Common functions
 */

/**
 * Returns the provider for a url.
 *
 * @param string $url
 *  Teh url to get the provider for.
 * @return mixed
 *  A valid callback or FALSE
 */
function micropublisher_oembed_get_provider($url, &$matches) {
  $host = _micropublisher_oembed_get_host($url);
  if ($host) {
    $providers = micropublisher_oembed_providers($host);
    foreach ($providers as $regex => $info) {
      if (preg_match($regex, $url, $matches)) {
        return $info;
      }
    }
  }
  return FALSE;
}

/**
 * A utility function to get the base domain from a url.
 *
 * @param string $uri
 *  The uri to get the domain form
 * @return string
 *  The domain or NULL
 */
function _micropublisher_oembed_get_host($uri) {
  $matches = array();
  if (preg_match('/^https?\:\/\/([^\/]+)/', $uri, $matches)) {
    $matches = explode('.', $matches[1]);
    $match_count = count($matches);
    if ($match_count > 1) {
      return $matches[$match_count - 2] . '.' . $matches[$match_count - 1];
    }
    else {
      return $matches[0];
    }
  }
  return NULL;
}

/**
 * Returns all the registered providers, or the providers for a specific host.
 *
 * @param string $host
 *  Optional. Supply a hostname if you only want the provider patterns for a specific host.
 * @return array
 */
function micropublisher_oembed_providers($url_host = NULL) {
  static $providers;

  if (!$providers) {
    $cache_key = 'oembed:micropublisher:providers';

    if (!($cache = cache_get($cache_key)) && isset($cache->data)) {
      $providers = $cache->data;
    }
    else {
      $providers = array();
      $modules = module_implements('oembedprovider');
      foreach ($modules as $module) {
        $ps = call_user_func($module . '_oembedprovider');
        foreach ($ps as $pattern => $info) {
          $host = _micropublisher_oembed_get_host($info->scheme);
          $regex_pattern = '/' . str_replace('\*', '(.+)', preg_quote($pattern, '/')) . '/';
          $providers[$host][$regex_pattern] = $info;
        }
      }
      drupal_alter('oembedprovider', $providers);
      foreach ($providers as $host => &$patterns) {
        uksort($patterns, '_micropublisher_oembed_specificity_compare');
      }
      cache_set($cache_key, $providers);
    }
  }

  if ($url_host) {
    return isset($providers[$url_host]) ? $providers[$url_host] : array();
  }
  return $providers;
}

/**
 * Fetch data for an embeddable URL.
 *
 * @param string $url
 *   An external URL for the content to embed.
 * @param array $attributes
 *   An associative array of attributes, with the following keys:
 *   - 'maxwidth'
 *       The maximum width of the embed, in pixels.
 *   - 'maxheight'
 *       The maximum height of the embed, in pixels.
 * @return
 *   False or an object representing the embeddable data of the URL.
 */
function micropublisher_oembed_data($url, $attributes = array()) {
  $matches = array();
  if ($provider = (array) micropublisher_oembed_get_provider($url, $matches)) {
    return micropublisher_oembed_fetch($provider, $url, $matches, $attributes);
  }
  return FALSE;
}

/**
 * Fetch data for an embeddable URL.
 */
function micropublisher_oembed_fetch($provider, $url, $matches, $attributes = array()) {
  global $_oembed_default_attributes;

  $embed = FALSE;

  if (!empty($_oembed_default_attributes)) {
    $attributes = array_merge($_oembed_default_attributes, $attributes);
  }
  $attributes['url'] = $url;
  $query = http_build_query($attributes, NULL, '&');

  $source = isset($provider['callback']) ? $provider['callback'] : $provider['endpoint'];
  $cache_key = 'oembed:micropublisher:embed:' . md5($source . $url . $query);
  $cache = cache_get($cache_key);

  if ($cache && isset($cache->data)) {
    $embed = $cache->data;
  }
  else {
    if (!empty($provider['callback'])) {
      $embed = call_user_func($provider['callback'], $provider, $url, $matches, $attributes);
      if ($embed) {
        $embed = (object) $embed;
      }
    }
    else {
      $fetch_url = $provider['endpoint'] . '?' . $query;

      //TODO: Add alternative ways of fetching the content - like http client?
      $response = drupal_http_request($fetch_url);
      if (!$response->error) {
        $embed = json_decode($response->data);
        if (!is_object($embed)) {
          try {
            $embed = @new SimpleXMLElement($response->data);
            $embed = (object) get_object_vars($embed);
            if (!is_string($embed->title)) {
              $embed->title = '';
            }
          } catch (Exception $e) {
            watchdog('micropublisher', 'Could not parse response from %url.', array('%url' => $fetch_url), WATCHDOG_ERROR);
          }
        }

        if (empty($embed->version) || empty($embed->type) || intval($embed->version) != 1) {
          $embed = FALSE;
          watchdog('micropublisher', 'Response from %url not a valid oEmbed response.', array('%url' => $fetch_url), WATCHDOG_ERROR);
        }
      }
      else {
        watchdog('micropublisher', 'Error fetching data from %url.', array('%url' => $fetch_url), WATCHDOG_ERROR);
      }
    }

    if ($embed) {
      $embed->original_url = $url;
    }

    $max_age = isset($embed->cache_age) ? intval($embed->cache_age) : empty($provider['callback']) ? 600 : 60;
    cache_set($cache_key, $embed, 'cache', time() + $max_age);
  }

  return $embed;
}
