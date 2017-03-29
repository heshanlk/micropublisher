<?php

/**
 * @file
 * Implement twitpic of provider_interface interface
 */
class twitpic extends provider_interface {

  public function __construct($uri) {
    parent::__construct($uri);
  }

  public function fetch($provider, $matches, $attributes = array()) {
    $embedded = parent::fetch($provider, $matches, $attributes);
    $micropublisher = new stdClass();
    $micropublisher->uri = $embedded->original_url;
    $micropublisher->title = !empty($embedded->title)? $embedded->title : '';
    $micropublisher->description = NULL;
    $micropublisher->html = NULL;
    $micropublisher->author_name = '';
    $micropublisher->provider = $embedded->provider_name;
    $micropublisher->image = $this->get_local_thumbnail($embedded->url);
    $micropublisher->images = array($micropublisher->image);
    $micropublisher->type = 'image';
    return $micropublisher;
  }

  public function get_local_thumbnail($image_url) {
    return parent::get_local_thumbnail($image_url);
  }

}
