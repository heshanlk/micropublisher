<?php

/**
 * @file
 * Implemnt viddler of provider_interface interface
 */
class viddler extends provider_interface {

  public function __construct($url) {
    parent::__construct($url);
  }

  public function fetch($provider, $matches, $attributes = array()) {
    $embedded = parent::fetch($provider, $matches, $attributes);
    $micropublisher = new stdClass();
    $micropublisher->uri = $embedded->original_url;
    $micropublisher->title = $embedded->title;
    $micropublisher->html = $embedded->html;
    $micropublisher->author_name = $embedded->author_name;
    $micropublisher->provider = $embedded->provider_name;
    $micropublisher->image = NULL;
    $micropublisher->images = array($micropublisher->image);
    $micropublisher->type = $embedded->type;
    return $micropublisher;
  }

  public function get_local_thumbnail($image_url) {
    return parent::get_local_thumbnail($image_url);
  }

}

