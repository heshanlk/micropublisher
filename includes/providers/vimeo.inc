<?php

/**
 * @file
 * Implement vimeo of provider_interface interface
 */
class vimeo extends provider_interface {

  public function __construct($url) {
    parent::__construct($url);
  }

  public function fetch($provider, $matches, $attributes = array()) {
    // array('maxwidth' => 380, 'maxheight' => 230) was not work
    $attributes += array('width' => 380, 'height' => 230);
    $embedded = parent::fetch($provider, $matches, $attributes);
    $micropublisher = new stdClass();
    $micropublisher->uri = $embedded->original_url;
    $micropublisher->title = $embedded->title;
    $micropublisher->description = $embedded->description;
    $micropublisher->html = $embedded->html;
    $micropublisher->author_name = $embedded->author_name;
    $micropublisher->provider = $embedded->provider_name;
    $micropublisher->image = $this->get_local_thumbnail($embedded->thumbnail_url);
    $micropublisher->images = array($micropublisher->image);
    $micropublisher->type = $embedded->type;
    return $micropublisher;
  }

  public function get_local_thumbnail($image_url) {
    return parent::get_local_thumbnail($image_url);
  }

}
