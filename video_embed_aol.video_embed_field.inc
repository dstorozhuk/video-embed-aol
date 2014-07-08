<?php

/**
 * API Info for video_embed_field module
 */

function video_embed_aol_field_info_alter(&$info) {
  if (isset($info['video_embed_field'])) {
    $info['video_embed_field']['instance_settings']['use_id'] = 0;
  }
}

/**
 * Implements of hook_form_field_ui_field_edit_form_alter().
 */
function video_embed_aol_form_field_ui_field_edit_form_alter(&$form, &$form_state, $form_id) {
  $instance = $form['#instance'];
  $form['instance']['settings']['use_id'] = array(
    '#type' => 'radios',
    '#title' => t('Use video ID instead of URL'),
    '#default_value' => $instance['settings']['use_id'],
    '#options' => array(t('No'), t('Yes')),
  );
}

/**
 * Implements of hook_video_embed_handler_info().
 */
function video_embed_aol_video_embed_handler_info() {
  $handlers = array();

  $handlers['aol'] = array(
    'title' => 'AOL ON',
    'function' => 'video_embed_aol_handle_aol',
    'thumbnail_function' => 'video_embed_aol_handle_aol_thumbnail',
    'data_function' => 'video_embed_aol_handle_aol_data',
    'form' => 'video_embed_aol_handle_aol_form',
    'domains' => array(
      'on.aol.com'
    ),
    'defaults' => array(
      "sid"=>'',
      "skey"=>'',
      "add_ons"=>0,
      "auto_start"=>0,
      "country"=>"",
      "domain"=>'all',
      "num_related_return"=>10,
      "transcript"=>0,
      "video_language"=>'en',
      'width' => 480,
      'height' => 401,
    ),
  );

  return $handlers;
}

/**
 * Implements of 'function' callback.
 *
 * @see video_embed_aol_video_embed_handler_info().
 */
function video_embed_aol_handle_aol($url, $settings) {
  $video_info = _video_embed_aol_extract_aol_video_data_from_url($url, $settings);
  dpm($video_info);
  if (isset($video_info['player'])) {
    return array(
      '#markup' => $video_info['player']['source'],
    );
  }
  return '';
}

/**
 * Implements of 'thumbnail_function' callback.
 *
 * @see video_embed_aol_video_embed_handler_info().
 */
function video_embed_aol_handle_aol_thumbnail($url) {
  $aol_video_data = _video_embed_aol_extract_aol_video_data_from_url($url);
  $return = array(
    'id' => '',
    'url' => ''
  );
  if ($aol_video_data) {
    $return = array(
      'id' => $aol_video_data['id'],
      'url' => $aol_video_data['image'],
    );
  }
  return $return;
}

/**
 * A forms api callback, returns the settings form for the provider
 * @param $defaults - default/current values for your provider, the currently saved settings
 *                       with empty values filled with the defaults provided in info hook
 * @return a form as defined by forms api
 *
 * @see http://api.drupal.org/api/drupal/developer!topics!forms_api_reference.html/7
 */
function video_embed_aol_handle_aol_form($defaults) {
  $form = array();
  module_load_include('inc', 'video_embed_aol', 'video_embed_aol.admin');
  include_once DRUPAL_ROOT . '/includes/locale.inc';
  $country_list = country_get_list();
  $languages = array(
    'en' => t('English'),
    'cn' => t('Chinese'),
    'fr' => t('French'),
    'jp' => t('Japanese'),
    'es' =>t('Spanish'),
    'de' => t('German/Deutch'),
    'any' => t('Any language')
  );
  $yes_no_options = array(t('No'), t('Yes'));

  $settings = array(
    "height"                 => array(
      "#description" => t("Embed height value"),
      "#type"        => "textfield"
    ),
    "width"                  => array(
      "#description" => t("Embed width value (Default 480)"),
      "#type"        => "textfield",
    ),
    "sid"                    => array(
      "#description" => t("Account associated syndicator ID"),
      "#type"        => "textfield"
    ),
    "skey"                   => array(
      "#description" => t("Setting key"),
      "#type"        => "textfield"
    ),
    "add_ons"                => array(
      "#description" => t("Include/exclude add-ons for video"),
      "#type"        => "select",
      '#options' => $yes_no_options,
    ),
    "auto_start"             => array(
      "#description" => t("The video starts playing automatically"),
      "#type"        => "select",
      '#options' => $yes_no_options,
    ),
    "country"                => array(
      "#description" => t("Filter the results to include only videos which are playable in the chosen country"),
      "#type"        => "select",
      '#options' => $country_list
    ),
    "domain"                 => array(
      "#description" => t("Account associated domain. If empty then 'all'"),
      "#type"        => "textfield"
    ),
    "num_related_return"     => array(
      "#description" => t("Number of related videos to return per video."),
      "#type"        => "select",
      '#options' => drupal_map_assoc(range(0, 20)),
    ),
    "transcript"             => array(
      "#description" => t("Include/exclude transcript for video"),
      "#type"        => "select",
      '#options' => $yes_no_options,
    ),
    "video_language"         => array(
      "#description" => t("Filter by language (Default is English)."),
      "#type"        => "select",
      '#options' => $languages,
    ),
  );

  foreach ($settings as $field_name => $field) {
    $form[$field_name]['#title'] = t(drupal_ucfirst(str_replace('_', ' ', $field_name)));
    $form[$field_name]['#default_value'] = $defaults[$field_name];
    $form[$field_name] += $field;
  }
  return $form;
}

/**
 * Implements callback "data_function".
 *
 * @see video_embed_aol_video_embed_handler_info().
 */
function video_embed_aol_handle_aol_data($url) {
  $aol_video_data = _video_embed_aol_extract_aol_video_data_from_url($url);
  return $aol_video_data;
}

/**
 * Implements of hook_field_widget_form_alter().
 */
function video_embed_aol_field_widget_form_alter(&$element, &$form_state, $context) {

  if ($context['field']['type'] == 'video_embed_field') {
    $instance = $context['instance'];
    $element['#element_validate'][] = 'video_embed_aol_video_url_field_validate';
    $element['#context'] = $context;
    if ($instance['settings']['use_id']) {
      $video_info = array('id' => '');
      if (isset($context['items'][$context['delta']]['video_data'])) {
        $video_info = unserialize($context['items'][$context['delta']]['video_data']);
      }

      $element['video_id'] = array(
        '#type' => 'textfield',
        '#title' => t('AOL Video ID'),
        '#element_validate' => array('video_embed_aol_video_id_field_validate'),
        '#default_value' => $video_info['id'],
        '#field' => $context['field'],
        '#instance' => $instance
      );
      hide($element['video_url']);
    }
  }
}

/**
 * Validate function.
 */
function video_embed_aol_video_id_field_validate($element, &$form_state, $form){
  $video_id = drupal_array_get_nested_value($form_state['values'], $element['#parents']);

  if (empty($video_id)) {
    return;
  }

  $video_info = _get_aol_video_info($video_id);
  if ($video_info) {
    $video_url_element_parents = array('#parents' => $element['#parents']);
    $video_url_element_parents['#parents'][count($element['#parents']) - 1] = 'video_url';
    form_set_value($video_url_element_parents, $video_info['player']['url'], $form_state);
    form_set_value($video_url_element_parents, $video_info['player']['url'], $form_state);
  }
  else {
    drupal_set_message(t('Field %name: enable to get information from AOL. Please, try again. ', array('%name' => $element['#title'])), 'error');
  }
}

function video_embed_aol_video_url_field_validate($element, &$form_state, $form) {
  $context = $element['#context'];
  dpm($context, '$context');
}
