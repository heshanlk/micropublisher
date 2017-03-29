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

<div class="micropublisher_embedded_images micropublisher_preview">
  <input type="hidden" value="1" id ="micropublisher-embedded-img-active"/>
  <input type="hidden" value="<?php print (!empty($embedded->images)) ? count($embedded->images) : 0; ?>" id ="micropublisher-embedded-img-total"/>
  <?php if (!$page): ?>
  <?php $i = 1;
    if (!empty($embedded->images)): ?>
  <?php foreach ($embedded->images as $image) : ?>
        <img <?php print 'rel=' . $i;
        print ($i != 1) ? ' style="display: none;"' : ''; ?> uri ='<?php print $image ?>' src='<?php print file_create_url($image) ?>' width='100' id='micropublisher-embedded-img-<?php print $i; ?>' />
    <?php $i++; ?>
    <?php endforeach; ?>
    <?php endif; ?>
    <?php elseif (!empty($embedded->image)): ?>
        <img src='<?php print file_create_url($embedded->image) ?>' width='100' id='image' />
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
  <?php /* if (!$page): ?>
            <div style="float:left">
              <a href="#" onclick="javascript:Drupal.behaviors.micropublisherPrev(); return false;"><img src="/<?php print $path_to_images; ?>/prev.png" id="edit-micropublisher-embedded-prev" alt="" /></a>
              <a href="#" onclick="javascript:Drupal.behaviors.micropublisherNext(); return false;"><img src="/<?php print $path_to_images; ?>/next.png" id="edit-micropublisher-embedded-next" alt="" /></a>
            </div>
            <!-- Show total images -->
            <div class="micropublisher_embedded_totalimg">
    <?php print 'Total images ' . count($embedded->images) . '/1'; ?>
          </div>
  <?php endif; */ ?>
</div>