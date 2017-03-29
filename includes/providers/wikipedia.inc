<?php

/**
 * @file
 * Implement wikipedia of provider_interface interface
 */
class wikipedia extends provider_interface {

  public function __construct($uri) {
    parent::__construct($uri);
  }

  public function fetch($provider, $matches, $attributes = array()) {
    $embedded = parent::fetch($provider, $matches, $attributes);
    echo '<pre>';
    print_r($embedded);
    die();
    $micropublisher = new stdClass();
    $micropublisher->uri = $embedded->original_url;
    $micropublisher->title = $embedded->title;
    $micropublisher->description = NULL;
    $micropublisher->html = NULL;
    $micropublisher->author_name = $embedded->author_name;
    $micropublisher->provider = $embedded->provider_name;
    $micropublisher->image = $embedded->url;
    $micropublisher->images = array($micropublisher->image);
    $micropublisher->type = 'image';
    return $micropublisher;
  }

}