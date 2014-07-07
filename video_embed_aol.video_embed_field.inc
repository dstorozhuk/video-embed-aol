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
      'api.5min.com',
      'on.aol.com'
    ),
    'defaults' => array(
      'width' => 640,
      'height' => 360,
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
  $video_info = _video_embed_aol_extract_aol_video_data_from_url($url);
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
  $form += video_embed_aol_player_form(array(), $d =  array(), NULL, $defaults);
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
 * Helper function to retrieve data from AOL ON for a given video URL.
 * @param $url
 * The video page url.
 * @return mixed
 */
function _video_embed_aol_extract_aol_video_data_from_url($url) {
  $parsed = @parse_url($url);
  if ($parsed) {
    if (isset($parsed['path'])) {
      $exploded_path = explode('-', $parsed['path']);
      $posible_video_id = end($exploded_path);
      if (is_numeric($posible_video_id)) {
        $video_info = _get_aol_video_info($posible_video_id);
        return $video_info;
      }
    }
  }
  return FALSE;
}

/**
 * Implements of hook_field_widget_form_alter().
 */
function video_embed_aol_field_widget_form_alter(&$element, &$form_state, $context) {

  if ($context['field']['type'] == 'video_embed_field') {
    $instance = $context['instance'];
    if ($instance['settings']['use_id']) {
      $video_info = unserialize($context['items'][$context['delta']]['video_data']);
      $element['video_id'] = array(
        '#type' => 'textfield',
        '#title' => t('AOL Video ID'),
        '#element_validate' => array('video_embed_aol_field_validate'),
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
function video_embed_aol_field_validate($element, &$form_state, $form){
  $video_id = drupal_array_get_nested_value($form_state['values'], $element['#parents']);

  if (empty($video_id)) {
    return;
  }

  $video_info = _get_aol_video_info($video_id);
  if ($video_info) {
    $video_url_element_parents = array('#parents' => $element['#parents']);
    $video_url_element_parents['#parents'][count($element['#parents']) - 1] = 'video_url';
    form_set_value($video_url_element_parents, $video_info['player']['url'], $form_state);
  }
  else {
    drupal_set_message(t('Field %name: enable to get information from AOL. Please, try again. ', array('%name' => $element['#title'])), 'error');
  }

}
