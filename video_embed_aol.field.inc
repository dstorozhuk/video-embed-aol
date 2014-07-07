<?php

/**
 * Implements of hook_field_formatter_info().
 */
function video_embed_aol_field_formatter_info() {
  $formatters = array();

  $formatters['video_embed_aol_video'] = array(
    'label' => t('AOL ON Video'),
    'field types' => array('text'),
    'settings' => array(
      'aol_player' => 1, // Get first player from aol_player table.
    )
  );

  $formatters['video_embed_aol_video_playlist_id'] = array(
    'label' => t('AOL ON Video Play list Id'),
    'field types' => array('text'),
    'settings' => array(
      'aol_player' => 1, // Get first player from aol_player table.
    )
  );

  return $formatters;
}

/**
 * Implements hook_field_formatter_settings_form().
 */
function video_embed_aol_field_formatter_settings_form($field, $instance, $view_mode, $form, &$form_state) {
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];

  $element = array();
  if ($display['type'] == 'video_embed_aol_video' || $display['type'] == 'video_embed_aol_video_playlist_id') {
    $options = video_embed_aol_player_list();
    $options['default_image'] = '189x106 thumbnail';
    $element['aol_player'] = array(
      '#title' => t('Player'),
      '#type' => 'select',
      '#default_value' => isset($settings['aol_player']) ? $settings['aol_player'] : 1,
      '#options' => $options,
    );
  }

  return $element;
}

/**
 * Implements hook_field_formatter_settings_summary().
 */
function video_embed_aol_field_formatter_settings_summary($field, $instance, $view_mode) {
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];
  $players = video_embed_aol_player_list();
  if ($display['type'] == 'video_embed_aol_video' || $display['type'] == 'video_embed_aol_video_playlist_id') {
    $summary = t('Player Name: @PLAYER', array('@PLAYER' => $players[$settings['aol_player']]));

    return $summary;
  }
}

/**
 * Implements hook_field_formatter_view().
 */
function video_embed_aol_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display){
  $elements = array(0 => array());
  if ($display['type'] == 'video_embed_aol_video' || $display['type'] == 'video_embed_aol_video_playlist_id') {
    $player_data = video_embed_aol_player_load($display['settings']['aol_player']);
    if (!empty($player_data)) {
      $query_str = build_aol_js_query($player_data);
    }
    switch ($display['type']) {
      case 'video_embed_aol_video':
        $elements[0]['player'] = array(
          '#theme' => 'aol_player',
          'items' => array(),
        );
        // Set video id value.
        foreach ($items as $delta => $item) {
          $elements[0]['player']['items'][$delta] = array(
            'entity_type' => $entity_type,
            'field_name' => $field['field_name'],
            'sid' => variable_get("video_embed_aol_sid"),
            'nid' => $entity->nid,
            'video_id' => $item['value'],
            'player' => $display['settings']['aol_player'],
            'player_query' => $query_str,
            'player_width' => isset($player_data->width) ? $player_data->width : '',
            'player_height' => isset($player_data->height) ? $player_data->height : '',
          );
        }
        break;
      case 'video_embed_aol_video_playlist_id':
        $elements[0]['player'] = array(
          '#theme' => 'aol_playlist_player',
          'items' => array(),
        );
        // Set playlist id data.
        foreach ($items as $delta => $item) {
          $elements[0]['player']['items'][$delta] = array(
            'entity_type' => $entity_type,
            'field_name' => $field['field_name'],
            'sid' => variable_get("video_embed_aol_sid"),
            'playlist_id' => $item['value'],
            'player' => $display['settings']['aol_player'],
            'player_query' => $query_str . '&relatedMode=2&sequential=1&videoGroupID=' . $item['value'],
            'player_width' => isset($player_data->width) ? $player_data->width : '',
            'player_height' => isset($player_data->height) ? $player_data->height : '',
          );
        }
        break;
    }
  }

  return $elements;
}
