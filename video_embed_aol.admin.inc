<?php

/**
 * @file
 * Settings pages callbacks.
 */

/* Setting SID value */
function video_embed_aol_setting_form() {
  $form['video_embed_aol_sid'] = array(
    '#type'  => 'textfield',
    '#title' => 'AOL ON Syndicator ID',
    '#description' => t('AOL ON Syndicator ID value.  This is requirement for all video embeds on AMI site.'),
    '#size'  => 10,
    '#maxlength' => 10,
    '#default_value' => variable_get("video_embed_aol_sid"),
    '#required' => TRUE,
  );

  return system_settings_form($form);
}

/**
 * Menu custom callback to show player page.
 *
 * @see video_embed_aol_menu().
 *
 * @return array
 */
function video_embed_aol_player_page() {
  $players = video_embed_aol_player_load_all();

  $header = array(
    t('Player Name'),
    t('Player ID'),
    t('Size'),
    array(
      'data' => t('Operations'),
      'colspan' => 2,
    ),
  );

  $rows = array();
  foreach ($players as $player) {
    $rows[] = array(
      check_plain($player->name),
      check_plain($player->player),
      check_plain($player->width . "x" . $player->height),
      l(t('Edit'), 'admin/config/media/aol_on/players/' . $player->pid . '/edit'),
      l(t('Delete'), 'admin/config/media/aol_on/players/' . $player->pid . '/delete'),
    );
  }

  return array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
  );
}

/**
 * Menu callback.
 *
 * Render aol player delete interface.
 */
function video_embed_aol_player_delete_form($form, &$form_state, $player) {
  $form['pid'] = array(
    '#type' => 'value',
    '#value' => $player->pid,
  );

  return confirm_form($form,
    t('Are you sure that you want to delete "@name" player?', array(
      '@name' => $player->name,
    )),
    'admin/config/media/aol_on/players');
}

/**
 * Submit callback.
 *
 * Delete the player.
 */
function video_embed_aol_player_delete_form_submit($form, &$form_state) {
  video_embed_aol_player_delete($form_state['values']['pid']);
  $form_state['redirect'] = 'admin/config/media/aol_on/players';
}

/**
 * Page callback used to render aol admin interface.
 *
 * @see video_embed_aol_menu().
 */
function video_embed_aol_player_form($form, &$form_state, $player = NULL) {
  // Store current form data to storage.
  if (!empty($form_state['values'])) {
    $form_state['storage']['saved'] = $form_state['values'];
  }

  $form = array();

  $form['pid'] = array(
    '#type' => 'hidden',
    '#value' => !empty($player->pid) ? $player->pid : '',
  );

  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Player Name'),
    '#default_value' => !empty($player->name) ? $player->name : '',
    '#maxlength' => 255,
    '#required' => TRUE,
  );
  $form['player'] = array(
    '#type' => 'textfield',
    '#title' => t('Player Machine Name'),
    '#default_value' => !empty($player->player) ? $player->player : '',
    '#maxlength' => 50,
    '#required' => TRUE,
  );
  $form['description'] = array(
    '#type' => 'textfield',
    '#title' => t('Player description'),
    '#default_value' => !empty($player->description) ? $player->description : '',
    '#size' => 50,
  );
  $form['width'] = array(
    '#type' => 'textfield',
    '#title' => t('Player Width'),
    '#default_value' => !empty($player->width) ? $player->width : '',
    '#size' => 5,
    '#maxlength' => 4,
    '#required' => TRUE,
    '#allowed_range' => array('min' => 1, 'max' => 800),
    '#element_validate' => array('_form_numeric_value_check'),
  );
  $form['height'] = array(
    '#type' => 'textfield',
    '#title' => t('Player Height'),
    '#default_value' => !empty($player->height) ? $player->height : '',
    '#size' => 5,
    '#maxlength' => 4,
    '#required' => TRUE,
    '#allowed_range' => array('min' => 1, 'max' => 600),
    '#element_validate' => array('_form_numeric_value_check'),
  );
  $form['extra_setting'] = array(
    '#type' => 'fieldset',
    '#title' => 'Optional Player Settings',
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['extra_setting']['videoControlDisplayColor'] = array(
    '#type' => 'textfield',
    '#title' => t('Video Control Display Color'),
    '#description' => t('Controls color of menu bar. Default color is #191919'),
    '#default_value' => !empty($player->videoControlDisplayColor) ? $player->videoControlDisplayColor : '',
    '#maxlength' => 7,
    '#size' => 10,
  );
  $form['extra_setting']['colorPallet'] = array(
    '#type' => 'textfield',
    '#title' => t('Color Pallet'),
    '#description' => t('Controls color scheme of functions/buttons. e.g. #FFEB00'),
    '#default_value' => !empty($player->colorPallet) ? $player->colorPallet : '',
    '#maxlength' => 7,
    '#size' => 10,
  );
  $form['extra_setting']['topHeader'] = array(
    '#type' => 'textfield',
    '#title' => t('Top Header'),
    '#description' => t('Allows a partner to type in any text above the video player.  Requires an additional 20px to the height of the player in order to maintain aspect ration'),
    '#default_value' => !empty($player->topHeader) ? $player->topHeader : '',
    '#maxlength' => 25,
    '#size' => 25,
  );
  $form['extra_setting']['volume'] = array(
    '#type' => 'textfield',
    '#title' => 'Volume',
    '#description' => t('Controls the default player volume.  Value can be from 1 to 100 (Percent)'),
    '#default_value' => !empty($player->volume) ? $player->volume : '',
    '#maxlength' => 3,
    '#size' => 4,
    '#allowed_range' => array('min' => 1, 'max' => 100),
    '#element_validate' => array('_form_numeric_value_check'),
  );
  $form['extra_setting']['relatedMode'] = array(
    '#type' => 'radios',
    '#title' => 'Related Mode',
    '#description' => t('Determines the display style of the related videos displayed on the player'),
    '#default_value' => !empty($player->relatedMode) ? $player->relatedMode : '',
    '#options' => array(
      0 => 'none',
      1 => 'right hidden (default)',
      101 => "fixed right",
      2 => "fixed bottom with thumbnails (60px default)",
      3 => 'fixed bottom with thumbnails and video title'
    ),
  );
  $form['extra_setting']['relatedBottomHeight'] = array(
    '#type' => 'textfield',
    '#title' => 'Related Mode Bottom Height',
    '#description' => t('Controls the height of the bottom related video panes'),
    '#default_value' => !empty($player->relatedBottomHeight) ? $player->relatedBottomHeight : '',
    '#maxlength' => 3,
    '#size' => 4,
    '#element_validate' => array('_form_aol_on_related_bottom_height_check'),
  );
  $form['extra_setting']['shuffle'] = array(
    '#type' => 'radios',
    '#title' => 'Shuffle',
    '#description' => t('Controls whether the videos in a player are shuffled or not each load'),
    '#default_value' => !empty($player->shuffle) ? $player->shuffle : '',
    '#options' => array(0 => 'Shuffle', 1 => 'Do Not Shuffle (default)'),
  );
  $form['extra_setting']['sequential'] = array(
    '#type' => 'radios',
    '#title' => 'Sequential',
    '#description' => t('Controls whether the videos in the playList parameter automatically play one after the other'),
    '#default_value' => !empty($player->sequential) ? $player->sequential : '',
    '#options' => array(
      0 => 'No Sequential Play (default)',
      1 => 'Sequential Play'
    ),
  );
  $form['extra_setting']['isAP'] = array(
    '#type' => 'radios',
    '#title' => 'Auto-Progress',
    '#description' => t('This contorls whether or not the player auto-progresses regardless if there is a group of videos, such as playlist. If playlist reached the last video, Video Seed will run against that video and auto-progress to the most relavant video in the library'),
    '#default_value' => !empty($player->isAP) ? $player->isAP : '',
    '#options' => array('' => t('no setting (default)'), 0 => 'No', 1 => 'yes'),
  );
  $form['extra_setting']['isFP'] = array(
    '#type' => 'radios',
    '#title' => 'Fixed Playlist',
    '#description' => t('This controls where or not the playlist and related-pane are fixed based off of VideoSeed/Semantic matching against the 1st video in player.  If a manual or smart playlist from the console is already is being used, it will continue to use that'),
    '#default_value' => !empty($player->isFP) ? $player->isFP : '',
    '#options' => array('' => t('no setting (default)'), 0 => 'No', 1 => 'yes'),
  );

  // Advertisement Settings.
  $form['ads'] = array(
    '#type' => 'fieldset',
    '#title' => 'Advertisement Settings',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['ads']['hasCompanion'] = array(
    '#type' => 'radios',
    '#title' => 'Companion',
    '#description' => t('Set companion banner on or off'),
    '#default_value' => !empty($player->hasCompanion) ? $player->hasCompanion : '',
    '#options' => array('false' => 'off', 'true' => 'on (default)'),
  );
  $form['ads']['companionPos'] = array(
    '#type' => 'radios',
    '#title' => 'Companion Position',
    '#description' => t('Location of companion banner adjacent to player'),
    '#default_value' => !empty($player->companionPos) ? $player->companionPos : '',
    '#options' => array(
      'right' => 'right (default)',
      'left' => 'left',
      'top' => 'top',
      'bottom' => 'bottom'
    ),
  );
  $form['ads']['cbCustomID'] = array(
    '#type' => 'textfield',
    '#title' => t('Companion Banner Custom ID of &lt;div&gt; tag'),
    '#description' => t('Use instead of companionPos. Draws the companion banner in a custom element. e.g. &lt;div id="banner"&gt;&lt;/div&gt;'),
    '#default_value' => !empty($player->cbCustomID) ? $player->cbCustomID : '',
    '#size' => 50,
    '#maxlength' => 50,
  );
  $form['ads']['innerMargin'] = array(
    '#type' => 'select',
    '#title' => t('Inner Margin'),
    '#description' => t('Determines the number of pixels margin between player and companion banner'),
    '#default_value' => !empty($player->innerMargin) ? $player->innerMargin : '',
    '#options' => range(0, 10, 1),
  );

  // Video Seed Settings.
  $form['video_seed'] = array(
    '#type' => 'fieldset',
    '#title' => t('Video Seed Settings. (Semantic Matching)'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['video_seed']['categories'] = array(
    '#type' => 'textfield',
    '#title' => t('Categories'),
    '#description' => t('Defines which of AOL On categories it searches within. Valid AOL ON categories delimited with a comma. e.g. 78,80'),
    '#default_value' => !empty($player->categories) ? $player->categories : '',
    '#element_validate' => array('_form_aol_on_seed_value_check'),
  );
  $form['video_seed']['fallback'] = array(
    '#type' => 'textfield',
    '#title' => t('Fall Back'),
    '#description' => t('If no video seed results are found or for this page, will present fallback videos(default) or video of the day in defined category. Valid AOL ON categories delimited with a comma. e.g. 78,80'),
    '#default_value' => !empty($player->fallback) ? $player->fallback : '',
    '#element_validate' => array('_form_aol_on_seed_value_check'),
  );
  $form['video_seed']['url'] = array(
    '#type' => 'textfield',
    '#title' => t('URL'),
    '#description' => t('Pass the URL you want to Seed, use only if different than the page URL'),
    '#default_value' => !empty($player->url) ?  $player->url : '',
    '#element_validate' => array('_form_aol_on_url_check'),
  );

  $form['submit'] = array(
    '#id' => 'ami-aol-on-player-form-submit',
    '#type' => 'submit',
    '#value' => !empty($player->pid) ? t('Update') : t('Add'),
  );

  $form['delete'] = array(
    '#id' => 'ami-aol-on-player-form-delete',
    '#type' => 'submit',
    '#value' => t('Delete'),
    '#access' => !empty($player->pid) ? TRUE : FALSE,
  );

  return $form;
}


/*
 * Custom form element validation function.
 *
 * @see video_embed_aol_player_form().
 */
function _form_numeric_value_check($element, &$form_state) {
  if (!$element['#required'] && empty($element['#value'])) {
    return;
  }

  if (!is_numeric($element['#value'])) {
    form_error($element, t("Invalid value for '@NAME' field. value must be in numeric value", array('@NAME' => $element['#title'])));
  }

}

/*
 * Custom form element validation function.
 *
 * @see video_embed_aol_player_form().
 */
function _form_aol_on_related_bottom_height_check($element, &$form_state) {

  if (($form_state['values']['relatedMode'] == 2) || ($form_state['values']['relatedMode'] == 3)) {
    if (!is_numeric($element['#value'])) {
      if (empty($element['#value'])) {
        form_error($element, t("'@NAME' field valie is required.", array('@NAME' => $element['#title'])));
      }
      else {
        form_error($element, t("Invalid '@NAME' field value. It must be positive integer.", array('@NAME' => $element['#title'])));
      }
    }
    else {
      if ($element['#value'] < 0) {
        form_error($element, t("Invalid '@NAME' field value.  It must be positive integer.", array('@NAME' => $element['#title'])));
      }
    }
  }
  else {
    // Validate when value is presented.
    if (!$element['#required'] && empty($element['#value'])) {
      return;
    }

    if (!is_numeric($element['#value'])) {
      form_error($element, t("Invalid value for '@NAME' field. value must be in numeric value", array('@NAME' => $element['#title'])));
    }
  }
}

/*
 * Custom form element validation function.
 *
 * @see video_embed_aol_player_form().
 */
function _form_aol_on_url_check($element, &$form_state) {
  // Validate when value is presented.
  if (!$element['#required'] && empty($element['#value'])) {
    return;
  }

  if (!valid_url($element['#value'])) {
    if (!valid_url($element['#value'], TRUE)) {
      form_error($element, t("Invalid URL value for '@NAME' field", array('@NAME' => $element['#title'])));
    }
  }
}

/*
 * Custom form element validation function.
 *
 * @see video_embed_aol_player_form().
 */
function _form_aol_on_seed_value_check($element, &$form_state) {
  // Validate when value is presented.
  if (!$element['#required'] && empty($element['#value'])) {
    return;
  }

  // Remove space between number and comma.
  $form_values = explode(",", $element['#value']);
  $hasError = FALSE;
  foreach ($form_values as $value) {
    $value = trim($value);
    if (!is_numeric($value)) {
      form_error($name, t("Invalid AOL ON Category ID value for '@NAME' field.   It must be numeric value", array('@NAME' => $element['#title'])));
      $hasError = TRUE;
      break;
    }
    else {
      if (((int) $value < 1) || ((int) $value > 500)) {
        form_error($element, t("Invalid AOL ON Category ID value for '@NAME' field.  Valid range is 1 to 500", array('@NAME' => $element['#title'])));
        $hasError = TRUE;
        break;
      }
      else {
        $categories[] = (int) $value;
      }
    }
  }

  if (!$hasError) {
    $form_state['values'][$element['#title']] = implode(",", $categories);
  }
}

/**
 * Submit callback.
 *
 * @see video_embed_aol_player_form().
 */
function video_embed_aol_player_form_submit($form, &$form_state) {
  if ($form_state['clicked_button']['#id'] == 'ami-aol-on-player-form-submit') {
    $skip_fields = array(
      'op',
      'submit',
      'delete',
      'form_build_id',
      'form_token',
      'form_id',
      'name',
      'player',
      'description',
      'width',
      'height'
    );

    foreach ($form_state['values'] as $field_name => $field_value) {
      // Do not skip when field value is numeric.
      if (in_array($field_name, $skip_fields) || (empty($field_value) && !is_numeric($field_value))) {
        continue;
      }
      $settings[$field_name] = $field_value;
    }
    $form_state['values']['settings'] = $settings;
    video_embed_aol_player_save((object) $form_state['values']);
    $form_state['redirect'] = 'admin/config/media/aol_on/players';
  }
  elseif ($form_state['clicked_button']['#id'] == 'ami-aol-on-player-form-delete') {
    $pid = $form_state['values']['pid'];
    $form_state['redirect'] = 'admin/config/media/aol_on/players/' . $pid . '/delete';
  }
}
