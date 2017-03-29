<?php

/**
 * @file
 * Implement slideshare of provider_interface interface
 */
class slideshare extends provider_interface {

  public function __construct($uri) {
    parent::__construct($uri);
  }

  public function fetch($provider, $matches, $attributes = array()) {
    $embedded = parent::fetch($provider, $matches, $attributes);
    $micropublisher = new stdClass();
    $micropublisher->uri = $embedded->original_url;
    $micropublisher->title = $embedded->title;
    $micropublisher->description = NULL;
    $micropublisher->html = $embedded->html;
    $micropublisher->author_name = $embedded->author_name;
    $micropublisher->provider = $embedded->provider_name;
    $micropublisher->image = $this->get_local_thumbnail($embedded->thumbnail_url);
    $micropublisher->images = array($micropublisher->image);
    $micropublisher->type = 'slideshare';
    return $micropublisher;
  }

  public function get_local_thumbnail($image_url) {
    return parent::get_local_thumbnail($image_url);
  }

}