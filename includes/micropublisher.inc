<?php

/*
 * @file
 * Class file used to wrap micropublisher embedded functions
 */

class micropublisher {

  private $provider = NULL;
  private $url = NULL;
  private $host = NULL;

  public function __construct($url) {
    
    $response = drupal_http_request($url);
    if (isset($response->redirect_url)){
      $url = $response->redirect_url;
    }
    $this->url = $url;
    $provider = $this->get_provider();
    // create the instance
    if (!empty($provider->name)) {
      $provider = $provider->name;
    }
    // load the class file,
    // @todo add to .info file for lazy load
    module_load_include('inc', 'micropublisher', 'includes/providers/' . $provider);
    if (class_exists($provider)) {
      $this->provider = new $provider($url);
    }
    else {
      drupal_set_message(t('The provider is not configured properly.'), 'error');
    }
  }

  /**
   * Returns the provider for a url.
   *
   * @param string $url
   *  Teh url to get the provider for.
   * @return mixed
   *  A valid callback or FALSE
   */
  private function get_provider() {
    $url = $this->url;
    $matches = array();
    $host = $this->get_host();
    if ($host) {
      $providers = $this->get_providers_list($host);
      foreach ($providers as $regex => $info) {
        if (preg_match($regex, $url, $matches)) {
          return $info;
        }
      }
    }
    // @todo check for passing default value, this is bad to pass link for all failed providers
    $provider = new stdClass;
    $provider->disabled = TRUE; /* Edit this to true to make a default provider disabled initially */
    $provider->name = 'link';
    $provider->title = 'link';
    $provider->endpoint = 'http://anyurl';
    $provider->scheme = 'http://*.anyurl*';
    return $provider;
  }

  /**
   * A utility function to get the base domain from a url.
   *
   * @param string $uri
   *  The uri to get the domain form
   * @return string
   *  The domain or NULL
   */
  private function get_host() {
    $url = $this->url;
    $matches = array();
    if (preg_match('/^https?\:\/\/([^\/]+)/', $url, $matches)) {
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
  private function get_providers_list($url_host = NULL) {
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
            $host = $this->get_host($info->scheme);
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
  public function get_data($attributes = array()) {
    $matches = array();
    if ($provider = (array) $this->get_provider()) {
      return $this->provider->fetch($provider, $matches, $attributes);
    }
    return FALSE;
  }

  private function apply_tokens() {}
}

abstract class provider_interface {

  private $url;
  private $oembed_default_attributes = array(
    'width' => 480,
    'height' => 320
  );

  public function __construct($url) {
    $this->url = $url;
  }

  protected function fetch($provider, $matches, $attributes = array()) {
    $_oembed_default_attributes = $this->oembed_default_attributes;

    $embed = FALSE;
    $url = $this->url;

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
        if (!isset($response->error)) {
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

      cache_set($cache_key, $embed, 'cache', CACHE_PERMANENT);
    }
    return $embed;
  }

  public function get_local_thumbnail($image_url, $identifier = NULL) {
    // save always to public path
    $directory = 'public://micropublisher/images/';
    $local_path = file_create_filename(check_plain(pathinfo($image_url, PATHINFO_BASENAME)), $directory);
    if (!file_exists($local_path)) {
      $dirname = drupal_dirname($local_path);
      file_prepare_directory($dirname, FILE_CREATE_DIRECTORY | FILE_MODIFY_PERMISSIONS);
      @copy($image_url, $local_path);
    }
    return $local_path;
  }

}