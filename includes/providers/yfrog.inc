<?php

/**
 * @file
 * Implement yfrog of provider_interface interface
 */
class yfrog extends provider_interface {

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
    if ($embedded->type == 'video' && $embedded->html) {
      $micropublisher->html = '<embed src="' . $embedded->html . '" width="' . $embedded->width . '" height="' . $embedded->height . '" allowFullScreen="true" wmode="transparent" type="application/x-shockwave-flash"/>';
    }

    $micropublisher->author_name = '';
    $micropublisher->provider = $embedded->provider_name;

    if ($embedded->type == 'image') {
      $micropublisher->image = $this->get_local_thumbnail($embedded->url);
    }
    else {
      $micropublisher->image = $this->get_local_thumbnail($embedded->thumbnail_url);
    }
    $micropublisher->images = array($micropublisher->image);

    $micropublisher->type = $embedded->type;
    return $micropublisher;
  }

  public function get_local_thumbnail($image_url) {
    return parent::get_local_thumbnail($image_url);
  }

}
