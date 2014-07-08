<?php

/**
 * @file
 * API implementations for video_embed_field module.
 */

/**
 * Implements of hook_video_embed_handler_info().
 */
function video_embed_aol_video_embed_handler_info() {
  $handlers = array();

  $handlers['aol'] = array(
    'title'              => 'AOL ON',
    'function'           => 'video_embed_aol_handle_aol',
    'thumbnail_function' => 'video_embed_aol_handle_aol_thumbnail',
    'data_function'      => 'video_embed_aol_handle_aol_data',
    'form'               => 'video_embed_aol_handle_aol_form',
    'domains'            => array(
      'on.aol.com'
    ),
    'defaults'           => array(
      "sid"                => '',
      "skey"               => '',
      "add_ons"            => 0,
      "auto_start"         => 0,
      "country"            => '',
      "domain"             => 'all',
      "num_related_return" => 10,
      "transcript"         => 0,
      "video_language"     => 'en',
      'width'              => 480,
      'height'             => 401,
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
    'id'  => '',
    'url' => ''
  );
  if ($aol_video_data) {
    $return = array(
      'id'  => $aol_video_data['id'],
      'url' => $aol_video_data['image'],
    );
  }

  return $return;
}

/**
 * Implements callback "form".
 *
 * @see video_embed_aol_video_embed_handler_info().
 */
function video_embed_aol_handle_aol_form($defaults) {
  $form = array();
  include_once DRUPAL_ROOT . '/includes/locale.inc';
  $country_list = country_get_list();
  $languages = array(
    'en'  => t('English'),
    'cn'  => t('Chinese'),
    'fr'  => t('French'),
    'jp'  => t('Japanese'),
    'es'  => t('Spanish'),
    'de'  => t('German/Deutch'),
    'any' => t('Any language')
  );
  $yes_no_options = array(t('No'), t('Yes'));

  $settings = array(
    "height"             => array(
      "#description" => t("Embed height value"),
      "#type"        => "textfield"
    ),
    "width"              => array(
      "#description" => t("Embed width value (Default 480)"),
      "#type"        => "textfield",
    ),
    "sid"                => array(
      "#description" => t("Account associated syndicator ID"),
      "#type"        => "textfield"
    ),
    "skey"               => array(
      "#description" => t("Setting key"),
      "#type"        => "textfield"
    ),
    "add_ons"            => array(
      "#description" => t("Include/exclude add-ons for video"),
      "#type"        => "select",
      '#options'     => $yes_no_options,
    ),
    "auto_start"         => array(
      "#description" => t("The video starts playing automatically"),
      "#type"        => "select",
      '#options'     => $yes_no_options,
    ),
    "country"            => array(
      "#description" => t("Filter the results to include only videos which are playable in the chosen country"),
      "#type"        => "select",
      '#options'     => $country_list
    ),
    "domain"             => array(
      "#description" => t("Account associated domain. If empty then 'all'"),
      "#type"        => "textfield"
    ),
    "num_related_return" => array(
      "#description" => t("Number of related videos to return per video."),
      "#type"        => "select",
      '#options'     => drupal_map_assoc(range(0, 20)),
    ),
    "transcript"         => array(
      "#description" => t("Include/exclude transcript for video"),
      "#type"        => "select",
      '#options'     => $yes_no_options,
    ),
    "video_language"     => array(
      "#description" => t("Filter by language (Default is English)."),
      "#type"        => "select",
      '#options'     => $languages,
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
