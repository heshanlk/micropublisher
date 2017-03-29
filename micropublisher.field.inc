<?php

/**
 * @file
 * A link field using the Field API.
 */
/* * *************************************************************
 * Field Type API hooks
 * ************************************************************* */

/**
 * Implements hook_field_info().
 *
 * Provides the description of the field.
 */
function micropublisher_field_info() {
  return array(
    'micropublisher' => array(
      'label' => t('Micropublisher'),
      'description' => t('Micropublisher field to render an urls.'),
      'default_widget' => 'micropublisher_link',
      'default_formatter' => 'micropublisher_full',
      'instance_settings' => array(
        'micropublisher_providers' => array()
      )
    ),
  );
}

/**
 * Implements hook_field_settings_form().
 */
function micropublisher_field_settings_form($field, $instance, $has_data) {
  return array();
}

/**
 * Implements hook_field_instance_settings_form().
 */
function micropublisher_field_instance_settings_form($field, $instance) {
  $widget = $instance['widget'];
  $settings = $instance['settings'];
  $providers = get_providers_list();
  $options = array();
  foreach ($providers as $provider) {
//    dsm($provider);
    $options[$provider->name] = $provider->title;
  }
//  $options += array('link' => 'General Links');
//  dsm($instance);
  $form['micropublisher_providers'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Allowed Providers'),
    '#options' => $options,
    '#required' => TRUE,
    '#default_value' => !empty($settings['micropublisher_providers']) ? $settings['micropublisher_providers'] : array()
  );
  return $form;
}

/**
 * Implements hook_field_load().
 */
function micropublisher_field_load($entity_type, $entities, $field, $instances, $langcode, &$items, $page) {
  $sids = array();
  foreach ($entities as $id => $entity) {
    // Load the items from the micropublisher_managed table.
    foreach ($items[$id] as $delta => $item) {
      if (!empty($item['sid'])) {
        $sids[] = $item['sid'];
      }
    }
  }

  $statuss = micropublisher_load_multiple($sids);

  foreach ($entities as $id => $entity) {
    foreach ($items[$id] as $delta => $item) {
      // If the item does not exist, mark the entire item as empty.
      if (empty($item['sid']) || !isset($statuss[$item['sid']])) {
        $items[$id][$delta] = NULL;
      }
      else {
        $items[$id][$delta] = array_merge($item, (array) $statuss[$item['sid']]);
      }
    }
  }
}

/**
 * Implements hook_field_prepare_view().
 */
function micropublisher_prepare_view($entity_type, $entities, $field, $instances, $langcode, &$items) {
  // @todo :Remove items specified to not be displayed.
}

/**
 * Implements hook_field_presave().
 */
function micropublisher_field_presave($entity_type, $entity, $field, $instance, $langcode, &$items) {
  // change the type
//  $entity->type = 'status';
  // Make sure that each item which will be saved with this object has a
  // permanent status, so that it will not be removed when temporary items are
  foreach ($items as $item) {
    $url = db_query("SELECT *
      FROM {micropublisher_managed} mm WHERE mm.sid = :sid", array(':sid' => $item['sid']))
        ->fetch();
    if (!$url->status) {
      $url->status = 1;
      drupal_write_record('micropublisher_managed', $url, 'sid');
    }
  }
}

/**
 * Implements hook_field_validate().
 */
function micropublisher_field_validate($entity_type, $entity, $field, $instance, $langcode, $items, &$errors) {
  //@todo : validations for url
  // Don't try to validate if there were any errors before this point
  // since the element won't have been munged back into a date.
  if (!form_get_errors()) {
//    foreach ($items as $delta => $item) {
//      if (is_array($item) && !empty($item)) {
//        if (!micropublisher_validate_url($item['url']))
//          $errors[$field['field_name']][$langcode][$delta][] = array(
//            'error' => 'value',
//            'message' => t("The URL is not a valida url for %field #%delta.", array('%delta' => $field['cardinality'] ? intval($delta + 1) : '', '%field' => t($instance['label']))),
//          );
//      }
//    }
  }
}

/**
 * Implements hook_field_insert().
 */
function micropublisher_field_insert($entity_type, $entity, $field, $instance, $langcode, &$items) {
  list($id, $vid, $bundle) = entity_extract_ids($entity_type, $entity);
}

/**
 * Implements hook_field_update().
 *
 * Checks for items that have been removed from the object.
 */
function micropublisher_field_update($entity_type, $entity, $field, $instance, $langcode, &$items) {
  list($id, $vid, $bundle) = entity_extract_ids($entity_type, $entity);
}

/**
 * Implements hook_field_delete().
 */
function micropublisher_field_delete($entity_type, $entity, $field, $instance, $langcode, &$items) {
  list($id, $vid, $bundle) = entity_extract_ids($entity_type, $entity);
  // Delete all micropublisher usages within this entity.
  foreach ($items as $delta => $item) {
    // @todo :Decrement the item usage count by field tables 1 and delete the item if possible.
    if (micropublisher_delete($item)) {
      $items[$delta] = NULL;
    }
  }
}

/**
 * Implements hook_field_delete_revision().
 */
function micropublisher_field_delete_revision($entity_type, $entity, $field, $instance, $langcode, &$items) {
  list($id, $vid, $bundle) = entity_extract_ids($entity_type, $entity);
  foreach ($items as $delta => $item) {
    // @todo :Decrement the item usage count by field tables 1 and delete the item if possible.
    if (micropublisher_delete($item)) {
      $items[$delta] = NULL;
    }
  }
}

/**
 * Implements hook_field_is_empty().
 */
function micropublisher_field_is_empty($item, $field) {
  return empty($item['sid']);
}

/* * *********************************************************************
 *  Field Type API: Formatter
 *
 *  These are the api hooks that present formatted (themed) output to the
 *  user.
 * ******************************************************************** */

/**
 * Implements hook_field_formatter_info().
 */
function micropublisher_field_formatter_info() {
  return array(
    'micropublisher_full' => array(
      'label' => t('Default'),
      'field types' => array('micropublisher'),
      'settings' => array('micropublisher_style' => '', 'micropublisher_link' => ''),
    ),
    'micropublisher_image' => array(
      'label' => t('Image only'),
      'field types' => array('micropublisher'),
      'settings' => array('micropublisher_style' => '', 'micropublisher_link' => ''),
    ),
    'micropublisher_video' => array(
      'label' => t('Full embed video'),
      'field types' => array('micropublisher')
    ),
  );
}

/**
 * Implements hook_field_formatter_settings_form().
 */
function micropublisher_field_formatter_settings_form($field, $instance, $view_mode, $form, &$form_state) {
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];

  $image_styles = image_style_options(FALSE);
  $element['micropublisher_style'] = array(
    '#title' => t('Micropublisher image style'),
    '#type' => 'select',
    '#default_value' => $settings['micropublisher_style'],
    '#empty_option' => t('None (original image)'),
    '#options' => $image_styles,
  );

  $link_types = array(
    'content' => t('Content'),
    'file' => t('Original URL'),
  );
  $element['micropublisher_link'] = array(
    '#title' => t('Link micropublisher image to'),
    '#type' => 'select',
    '#default_value' => $settings['micropublisher_link'],
    '#empty_option' => t('Nothing'),
    '#options' => $link_types,
  );

  return $element;
}

/**
 * Implements hook_field_formatter_settings_summary().
 */
function micropublisher_field_formatter_settings_summary($field, $instance, $view_mode) {
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];
  $micropublisher_style = isset($settings['micropublisher_style']) ? $settings['micropublisher_style'] : '';
  $micropublisher_link = isset($settings['micropublisher_link']) ? $settings['micropublisher_link'] : '';
  $summary = array();

  $image_styles = image_style_options(FALSE);
  // Unset possible 'No defined styles' option.
  unset($image_styles['']);
  // Styles could be lost because of enabled/disabled modules that defines
  // their styles in code.
  if (isset($image_styles[$micropublisher_style])) {
    $summary[] = t('Micropublisher image style: @style', array('@style' => $image_styles[$micropublisher_style]));
  }
  else {
    $summary[] = t('Original image');
  }

  $link_types = array(
    'content' => t('Linked to content'),
    'file' => t('Linked to Original URL'),
  );
  // Display this setting only if image is linked.
  if (isset($link_types[$micropublisher_link])) {
    $summary[] = $link_types[$micropublisher_link];
  }

  return implode('<br />', $summary);
}

/**
 * Implements hook_field_formatter_view().
 */
function micropublisher_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
  $element = array();

  if (isset($display['settings']['micropublisher_link'])) {
    // Check if the formatter involves a link.
    if ($display['settings']['micropublisher_link'] == 'content') {
      $uri = entity_uri($entity_type, $entity);
    }
    elseif ($display['settings']['micropublisher_link'] == 'file') {
      $link_file = TRUE;
    }
  }

  // set the display
  $theme = $display['type'];
  foreach ($items as $delta => $item) {
    if (isset($link_file)) {
      $uri = array(
        'path' => $item['uri'],
        'options' => array(),
      );
    }
    $element[$delta] = array(
      '#theme' => $theme,
      '#item' => $item,
      '#micropublisher_style' => isset($display['settings']['micropublisher_style'])? $display['settings']['micropublisher_style']: 'default',
      '#path' => isset($uri) ? $uri : '',
      '#entity' => $entity,
      '#field' => $field,
      '#instance' => $instance
    );
  }
  return $element;
}

/*
 * Renders the micropublisher full as a link to the node page.
 */

function theme_micropublisher_full($variables) {
  $item = $variables['item'];
  $micropublisher = NULL;
  drupal_add_css(drupal_get_path('module', 'micropublisher') . '/micropublisher.css');
  $micropublisher = (object) $item;
  // get the image from styles if any styles
  if ($variables['path']) {
    $image = theme('micropublisher_image', array('item' => $item, 'path' => $variables['path'], 'micropublisher_style' => $variables['micropublisher_style']));
  }
  else {
    $image = '';
  }
  if (isset($micropublisher->type))
    return theme('micropublisher_embedded_' . $micropublisher->type, array('url' => $micropublisher->uri, 'embedded' => $micropublisher, 'page' => TRUE, 'image' => $image));
}

/*
 * Renders the micropublisher thumbnail as a link to the node page.
 */

function theme_micropublisher_image($variables) {
  $item = $variables['item'];
// Inside a view $variables may contain null data. In that case, just return.
  if (empty($variables['item']['sid'])) {
    return '<!-- File not found: ' . $variables['item']['sid'] . ' -->';
  }

//setup our thumbnail object
  $image = array(
    'path' => $item['image'],
    'alt' => !empty($item['alt']) ? $item['alt'] : '',
    'options' => array(),
  );
// Do not output an empty 'title' attribute.
  if (drupal_strlen($item['title']) > 0) {
    $image['title'] = $item['title'];
  }

  if ($variables['micropublisher_style']) {
    $image['style_name'] = $variables['micropublisher_style'];
    $output = theme('image_style', $image);
  }
  else {
    $output = theme('image', $image);
  }

  if ($variables['path']) {
    $path = $variables['path']['path'];
    $options = $variables['path']['options'];
    // When displaying an image inside a link, the html option must be TRUE.
    $options['html'] = TRUE;
    $output = l($output, $path, $options);
  }
  return $output;
}

/*
 * Renders the micropublisher full as a link to the node page.
 */

function theme_micropublisher_video($variables) {
  $item = $variables['item'];
  $micropublisher = NULL;
  drupal_add_css(drupal_get_path('module', 'micropublisher') . '/micropublisher.css');
  $micropublisher = (object) $item;
  return $micropublisher->html;
}

/* * ************************************************************************
 * Field Type API: Widget
 *
 * The widget is the form element used to receive input from the user
 * when the field is being populated.
 * ************************************************************************ */

/**
 * Implements hook_field_widget_info().
 */
function micropublisher_field_widget_info() {
  return array(
    'micropublisher_link' => array(
      'label' => t('Share link'),
      'field types' => array('micropublisher'),
    )
  );
}

/**
 * Implements hook_field_widget_settings_form().
 */
function micropublisher_field_widget_settings_form($field, $instance) {
  return array();
}

/**
 * Implements hook_field_widget_form().
 */
function micropublisher_field_widget_form(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {
  $providers = get_providers_list();
  $options = array();
  foreach ($providers as $provider) {
    $options[$provider->name] = $provider;
  }
  drupal_add_js(array('micropublisher' => array('providers' => $options)), 'setting');
  drupal_add_js(array('micropublisher' => array('activated_providers' => $instance['settings']['micropublisher_providers'])), 'setting');
  $defaults = array(
    'sid' => 0
  );
  // Retrieve any values set in $form_state, as will be the case during AJAX
  // rebuilds of this form.
  if (isset($form_state['values'])) {
    $path = array_merge($element['#field_parents'], array($field['field_name'], $langcode));
    $path_exists = FALSE;
    $values = drupal_array_get_nested_value($form_state['values'], $path, $path_exists);
    if ($path_exists) {
      $items = $values;
      drupal_array_set_nested_value($form_state['values'], $path, NULL);
    }
  }

  foreach ($items as $delta => $item) {
    $items[$delta] = array_merge($defaults, $items[$delta]);
    // Remove any items from being displayed that are not needed.
    if ($items[$delta]['sid'] == 0) {
      unset($items[$delta]);
    }
  }

  // Re-index deltas after removing empty items.
  $items = array_values($items);

  // Update order according to weight.
  $items = _field_sort_items($field, $items);

  // Essentially we use the micropublisher type, extended with some enhancements.
  $element_info = element_info('micropublisher');
  $element += array(
    '#type' => 'micropublisher',
    '#default_value' => isset($items[$delta]) ? $items[$delta] : $defaults,
    '#value_callback' => 'micropublisher_field_widget_value',
    '#process' => array_merge($element_info['#process'], array('micropublisher_field_widget_process')),
    // Allows this field to return an array instead of a single value.
    '#extended' => TRUE,
  );
  if ($field['cardinality'] == 1) {
    // If there's only one field, return it as delta 0.
    if (empty($element['#default_value']['sid'])) {

    }
    $elements = ($element);
  }
  else {
    // If there are multiple values, add an element for each existing one.
    $delta = -1;
    foreach ($items as $delta => $item) {
      $elements[$delta] = $element;
      $elements[$delta]['#default_value'] = $item;
      $elements[$delta]['#weight'] = $delta;
    }
    // And then add one more empty row for new uploads.
    $delta++;
    if ($field['cardinality'] == FIELD_CARDINALITY_UNLIMITED || $delta < $field['cardinality']) {
      $elements[$delta] = $element;
      $elements[$delta]['#default_value'] = $defaults;
      $elements[$delta]['#weight'] = $delta;
      $elements[$delta]['#required'] = ($element['#required'] && $delta == 0);
    }
    // The group of elements all-together need some extra functionality
    // after building up the full list (like draggable table rows).
    $elements['#micropublisher_delta'] = $delta;
    $elements['#theme'] = 'micropublisher_widget_multiple';
    $elements['#theme_wrappers'] = array('fieldset');
    $elements['#process'] = array('micropublisher_field_widget_process_multiple');
    $elements['#field_name'] = $element['#field_name'];
    $elements['#language'] = $element['#language'];
  }
  return $elements;
}

/**
 * Implements hook_field_widget_error().
 */
function micropublisher_field_widget_error($element, $error, $form, &$form_state) {
  switch ($error['error']) {
    case 'micropublisher_invalid':
      form_error($element, $error['message']);
      break;
  }
}

/**
 * An element #value callback for the micropublisher field type
 */
function micropublisher_field_widget_value($element, $input = FALSE, $form_state) {
  if ($input) {
    // Checkboxes lose their value when empty.
    // If the display field is present make sure its unchecked value is saved.
  }
  // We depend on the micropublisher element to handle fetch url.
  $return = micropublisher_element_value($element, $input, $form_state);

  // Ensure that all the required properties are returned even if empty.
  $return += array(
    'sid' => 0
  );
//  echo '<pre>';
//  print_r($return);
//  die();
  return $return;
}

/**
 * An element #process callback for the micropublisher field type.
 *
 * Expands the micropublisher type to include the description and display fields.
 */
function micropublisher_field_widget_process($element, &$form_state, $form) {
  module_load_include('inc', 'node', 'node.pages');
  $micropublisher = NULL;
  $item = $element['#value'];
  $item['sid'] = $element['sid']['#value'];

  $element['#theme'] = 'micropublisher_widget';

  $field = field_widget_field($element, $form_state);
  $instance = field_widget_instance($element, $form_state);
  $settings = $instance['widget']['settings'];

  $element['#attached']['css'][] = drupal_get_path('module', 'micropublisher') . '/micropublisher.css';
  $element['#attached']['js'][] = drupal_get_path('module', 'micropublisher') . '/micropublisher.js';
  // Adjust the AJAX settings so that on upload and remove of any individual
  // micropublisher, the entire group of micropublisher fields is updated together.
  if ($field['cardinality'] != 1) {
    // @todo : have a look why we need to flip away last element of this
    $parents = array_slice($element['#array_parents'], 0);
    $new_path = 'micropublisher/ajax/' . implode('/', $parents) . '/' . $form['form_build_id']['#value'];
    $field_element = drupal_array_get_nested_value($form, $parents);
    $new_wrapper = $field_element['#id'] . '-ajax-wrapper';
    foreach (element_children($element) as $key) {
      if (isset($element[$key]['#ajax'])) {
        $element[$key]['#ajax']['path'] = $new_path;
        $element[$key]['#ajax']['wrapper'] = $new_wrapper;
      }
    }
    //@todo : check for ajax wrapper
//    unset($element['#prefix'], $element['#suffix']);
  }
  // Add another submit handler to the upload and remove buttons, to implement
  // functionality needed by the field widget. This submit handler, along with
  // the rebuild logic in micropublisher_field_widget_form() requires the entire field,
  // not just the individual item, to be valid.
  foreach (array('upload_button', 'remove_button') as $key) {
    $element[$key]['#submit'][] = 'micropublisher_field_widget_submit';
    $element[$key]['#limit_validation_errors'] = array(array_slice($element['#parents'], 0));
  }
  // generate the content to display underneath the link
  // Add the additional fields to display thumbnail and summary texts.
  if (!empty($item['sid'])) {
    $url = micropublisher_load($item['sid']);
    if (preg_match('/^((http|https|public|private):\/\/[a-z0-9-]+(.[a-z0-9-]+)*(:[0-9]+)?(\/.*)?)$/i', $url->uri)) {
      module_load_include('inc', 'micropublisher', 'includes/micropublisher');
      $provider = new micropublisher($url->uri);
      $micropublisher = $provider->get_data(array('maxwidth' => 380, 'maxheight' => 230));
    }
  }
  // preview to the display
  if (isset($micropublisher->type)) {
    $element['preview'] = array(
      '#markup' => theme('micropublisher_embedded_preview', array('url' => $micropublisher->uri, 'embedded' => $micropublisher, 'page' => FALSE)),
      '#access' => (bool) $item['sid']
    );
  }
  $element['title'] = array(
    '#type' => 'hidden',
    '#value' => (isset($micropublisher->title)) ? $micropublisher->title : '',
    '#access' => (bool) $item['sid']
  );
  $element['description'] = array(
    '#type' => 'hidden',
    '#value' => isset($micropublisher->description) ? $micropublisher->description : '',
    '#access' => (bool) $item['sid']
  );
  $element['image'] = array(
    '#type' => 'hidden',
    '#default_value' => isset($micropublisher->image) ? $micropublisher->image : '',
    '#attributes' => array('class' => 'micropublisher_img_hidden'),
    '#access' => (bool) $item['sid'],
  );
  $element['provider'] = array(
    '#type' => 'hidden',
    '#value' => isset($micropublisher->provider) ? $micropublisher->provider : '',
    '#access' => (bool) $item['sid'],
  );
  $element['type'] = array(
    '#type' => 'hidden',
    '#value' => isset($micropublisher->type) ? $micropublisher->type : '',
    '#access' => (bool) $item['sid'],
  );
  $element['html'] = array(
    '#type' => 'hidden',
    '#value' => isset($micropublisher->html) ? $micropublisher->html : '',
    '#access' => (bool) $item['sid'],
  );
  return $element;
}

/**
 * Submit handler for upload and remove buttons of micropublisher fields.
 *
 * This runs in addition to and after micropublisher_element_file_submit().
 *
 * @see micropublisher_managed_file_submit()
 * @see micropublisher_field_widget_form()
 * @see micropublisher_field_widget_process()
 */
function micropublisher_field_widget_submit($form, &$form_state) {
  // During the form rebuild, micropublisher_field_widget_form() will create field item
  // widget elements using re-indexed deltas, so clear out $form_state['input']
  // to avoid a mismatch between old and new deltas. The rebuilt elements will
  // have #default_value set appropriately for the current state of the field,
  // so nothing is lost in doing this.
  $parents = array_slice($form_state['triggering_element']['#parents'], 0, -1);
  drupal_array_set_nested_value($form_state['input'], $parents, NULL);
}

/**
 * Returns HTML for an individual micropublisher status widget.
 *
 * @param $variables
 *   An associative array containing:
 *   - element: A render element representing the widget.
 *
 * @ingroup themeable
 */
function theme_micropublisher_widget($variables) {
  $element = $variables['element'];
  $output = '';
  // The "form-micropublisher-url" class is required for proper AJAX functionality.
  $output .= '<div class="micropublisher-widget form-micropublisher-url clearfix">';
  $output .= drupal_render_children($element);
  $output .= '</div>';
  return $output;
}

/**
 * Get all providers list
 */
function get_providers_list($url_host = NULL) {
  static $providers;

  if (!$providers) {
    $cache_key = 'oembed:micropublisher:providers';

    if (!($cache = cache_get($cache_key)) && isset($cache->data)) {
      $providers = $cache->data;
    }
    else {
      $providers = array();
      $modules = module_implements('oembedprovider');
      foreach ($modules as $module) {
        $ps = call_user_func($module . '_oembedprovider');
        foreach ($ps as $pattern => $info) {
//          $host = $this->get_host($info->scheme);
          $regex_pattern = '/' . str_replace('\*', '(.+)', preg_quote($pattern, '/')) . '/';
          $providers[$regex_pattern] = $info;
        }
      }
      drupal_alter('oembedprovider', $providers);
      cache_set($cache_key, $providers);
    }
  }

  if ($url_host) {
    return isset($providers[$url_host]) ? $providers[$url_host] : array();
  }
  return $providers;
}
