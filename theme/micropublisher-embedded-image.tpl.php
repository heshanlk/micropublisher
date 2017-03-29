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
  <?php if (!empty($embedded->image)): ?>
  <?php print $image ?>
  <?php endif; ?>
    <!-- provider Icon -->
    <div class="micropublisher-embedded-provider-img">
      <img title="<?php print $embedded->provider ?>" alt="<?php print $embedded->provider ?>" src='/<?php print $path_to_images . '/providers/' . $embedded->provider . '.png'; ?>' width='16' id='micropublisher-embedded-provider-img' />
    </div>
  </div>
  <div class="micropublisher_embedded_info">
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