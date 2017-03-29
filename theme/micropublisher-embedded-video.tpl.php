<?php

/**
 * @file
 * The theme system, which controls the output of Drupal.
 *
 * The theme system allows for nearly all output of the Drupal system to be
 * customized by user themes.
 */
?>
<?php
$path_to_images = drupal_get_path('module', 'micropublisher') . '/images';
?>
<div class="micropublisher_embedded_images">
  <div id="<?php print $embedded->sid ?>_img" class="micropublisher_embedded_thumbnail">
    <a  href="#" onclick="javascript:Drupal.behaviors.micropublisherPlayVideo(<?php print $embedded->sid ?>); return false;">
      <img class="micropublisher_embedded_video_play" src='/<?php print $path_to_images ?>/play.png' />
      <img class="micropublisher_embedded_image_<?php print $embedded->type ?>" src='<?php print file_create_url($embedded->image) ?>' width='100' id='' />
    </a>
  </div>
  <!-- embedded code for video -->
  <div id="<?php print $embedded->sid ?>_code" class="micropublisher_embedded_video_code">
    <?php print $embedded->html ?>
  </div>
  <!-- provider Icon -->
  <div class="micropublisher-embedded-provider-img">
    <img title="<?php print $embedded->provider ?>" alt="<?php print $embedded->provider ?>" src='/<?php print $path_to_images . '/providers/' . $embedded->provider . '.png'; ?>' width='16' id='micropublisher-embedded-provider-img' />
  </div>
</div>
<div id="<?php print $embedded->sid ?>_info" class="micropublisher_embedded_info">
  <a href="<?php print $url; ?>" target="_blank">
    <div class="micropublisher_embedded_title">
      <?php print $embedded->title ?>
    </div>
    <div class="micropublisher_embedded_url">
      <?php print $embedded->uri ?>
    </div>
  </a>
  <div class="micropublisher_embedded_desc">
    <?php print $embedded->description ?>
  </div>
</div>