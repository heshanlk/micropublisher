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
  <img src='<?php print $embedded->image ?>' width='100' id='' >
  <input type="hidden" name="total_images" id="total_images" value="" />
</div>
<div class="micropublisher_embedded_info">
  <div class="micropublisher_embedded_title">
    <?php print $embedded->title ?>
  </div>
  <br clear="all" />
  <div class="micropublisher_embedded_url">
    <?php print $embedded->uri ?>
  </div>
  <br clear="all" /><br clear="all" />
  <div class="micropublisher_embedded_desc">
    <?php print $embedded->description ?>
  </div>
  <br clear="all" /><br clear="all" />
  <div style="float:left">
    <img src="/<?php print $path_to_images; ?>/prev.png" id="prev" alt="" />
    <img src="/<?php print $path_to_images; ?>/next.png" id="next" alt="" />
  </div>
  <div class="micropublisher_embedded_totalimg">
		Total images
  </div>
  <br clear="all" />
</div>