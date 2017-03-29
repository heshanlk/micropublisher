<?php

/**
 * @file
 * Implement link of provider_interface interface
 */
class link extends provider_interface {

  private $uri;
  private $html;

  public function __construct($uri) {
    module_load_include('php', 'micropublisher', 'lib/simplehtmldom/simple_html_dom');
    $this->uri = $this->sanitize($uri);
    $this->html = file_get_html($this->uri);
  }

  public function fetch($provider, $matches, $attributes = array()) {
    $uri = $this->uri;
    $cache_key = 'oembed:micropublisher:embed:' . md5($uri);
    $cache = cache_get($cache_key);

    if ($cache && isset($cache->data)) {
      $micropublisher = $cache->data;
    }
    else {
      $micropublisher = new stdClass();
      $micropublisher->uri = $this->uri;
      $micropublisher->title = $this->get_title();
      $micropublisher->description = $this->get_decription();
      $micropublisher->html = NULL;
      $micropublisher->author_name = 'Anonymous';
      $micropublisher->provider = variable_get('site_name', 'Localhost');
      $micropublisher->images = $this->get_images();
      $micropublisher->image = !empty($micropublisher->images) && is_array($micropublisher->images) ? $micropublisher->images[0] : NULL;
      $micropublisher->type = 'link';
      cache_set($cache_key, $micropublisher, 'cache', CACHE_PERMANENT);
    }
    return $micropublisher;
  }

  private function sanitize($uri) {
    $uri = trim($uri);
    if (get_magic_quotes_gpc()) {
      $uri = stripslashes($uri);
    }
    $uri = strtr($uri, array_flip(get_html_translation_table(HTML_ENTITIES)));
    $uri = strip_tags($uri);
    $uri = htmlspecialchars($uri);
    return $uri;
  }

  //get the title
  private function get_title() {
    foreach ($this->html->find('title') as $e)
      return strip_tags($e);
  }

  // get description
  private function get_decription() {
    // @todo get description tag from the meta tags
    foreach ($this->html->find('meta') as $e) {
      if ($e->name == 'description')
        return $e->content;
    }
//      return strip_tags($e);
    foreach ($this->html->find('p') as $e)
      return strip_tags($e);
  }

  // get images
  private function get_images() {
    // @todo: download files b4 check size
    $images = array();
    foreach ($this->html->find('img') as $image) {
      if (@getimagesize($image->src)) {
        list($width, $height, $type, $attr) = getimagesize($image->src);
        if ($width >= 50 && $height >= 50) {
          $images[$image->src] = $this->get_local_thumbnail($image->src);
        }
      }
    }
    // if empty then guess as image url
    $url = $this->uri;
    if (empty($images) && @getimagesize($url)) {
      $images[$url] = $this->get_local_thumbnail($url);
    }
    return array_values($images);
  }

  public function get_local_thumbnail($image_url) {
    return parent::get_local_thumbnail($image_url);
  }

}