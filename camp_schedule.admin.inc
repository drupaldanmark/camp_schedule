<?php

/**
 * Schedule settings.
 */
function camp_schedule_settings_form() {
  $types = node_get_types();
  foreach ($types as $type) {
    $options[$type->type] = $type->name;
  }
  
  $form['camp_schedule_node_type'] = array(
    '#title' => t('Schedule Node Type'),
    '#type' => 'select', 
    '#options' => $options,
    '#default_value' => variable_get('camp_schedule_node_type', NULL),
  );
  $form['camp_schedule_alert_conflict'] = array(
    '#title' => t('Alert Conflicts'),
    '#description' => t('When saving schedule assignments, are multiple sessions allowed in the same time slot?'),
    '#type' => 'checkbox',
    '#default_value' => variable_get('camp_schedule_alert_conflict', FALSE),
  );
  return system_settings_form($form);
}

/**
 * Manage schedule form.
 */
function camp_schedule_manage_form(&$form_state) {
  $form = array();
  $form['#content_extra_fields'] = $type['extra'];
  $form['#pre_render'][] = 'content_alter_extra_weights';  
  module_load_include('inc', 'content', 'includes/content.node_form');
  module_load_include('inc', 'content', 'includes/content.admin');
  // Get all of our form options such as day, time, room.
  
  // Define the field names we'd like to render.
  $render = array('field_session_day', 'field_session_time', 'field_session_room');
  $type = variable_get('camp_schedule_node_type', 'session');
  $cck_fields = content_types($type);
  
  drupal_add_js("Drupal.behaviors.camp_schedule = function() {
    $('tr', '#camp-schedule-manage-form').hover(function() {
      $(this).find('td').css('background-color', '#FFFCCC');
    }, function() {
      $(this).find('td').css('background-color', 'transparent');      
    });
  }", 'inline');

  // Find all of our session nodes.
  $form['#tree'] = TRUE;

  foreach (camp_schedule_load_nodes() as $node) {
    // We need to put the node object in $form['#node'] so that content_field_form can use it to put together the field.
    if (isset($node->nid)) {
      $form['#node'] = $node;
      $form['nodes'][$node->nid]['title'] = array('#value' => l($node->title, 'node/'. $node->nid));

      foreach ($cck_fields['fields'] as $field_name => $field) {
        if (in_array($field_name, $render)) {
          $form['#field_info'][$field['field_name']] = $field;
          $form['nodes'][$node->nid] += content_field_form($form, $form_state, $field);
        }
      }
      $form['nodes'][$node->nid]['field_session_day']['#title'] = '';
      $form['nodes'][$node->nid]['field_session_room']['#title'] = '';
      $form['nodes'][$node->nid]['field_session_time']['#title'] = '';
    }
  }
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Save',
  );
  if (!isset($form['#after_build'])) {
    $form['#after_build'] = array();
  }
  $form['#after_build'][] = 'camp_schedule_manage_form_after_build';  
  return $form;
}

/**
 * "After build" callback for node forms. Register the fields you want to alter.
 */
function camp_schedule_manage_form_after_build($form, &$form_state) {
  foreach ($form['nodes'] as $nid => $field) {
    unset($form['nodes'][$nid]['field_session_room']['value']['#options']['bof1']);
    unset($form['nodes'][$nid]['field_session_room']['value']['#options']['bof2']);
    unset($form['nodes'][$nid]['field_session_room']['value']['#options']['bof3']);
    unset($form['nodes'][$nid]['field_session_room']['value']['#options']['bof4']);
  }
  return $form;
}

/**
 * Form theme function.
 */
function theme_camp_schedule_manage_form($form) {
  $header = array('Session Title', 'Day', 'Room', 'Time');
  if (is_array($form['nodes'])) {
    foreach ($form['nodes'] as $nid => $node) {
      if (is_numeric($nid)) {
        $rows[] = array(
          drupal_render($form['nodes'][$nid]['title']),
          drupal_render($form['nodes'][$nid]['field_session_day']),
          drupal_render($form['nodes'][$nid]['field_session_room']),
          drupal_render($form['nodes'][$nid]['field_session_time'])
        );
      }
    }
  }
  $output .= theme('table', $header, $rows);
  $output .= drupal_render($form);
  return $output;
}

/**
 * Validation callback.
 */
function camp_schedule_manage_form_validate($form, &$form_state) {
  $alert_conflict = variable_get('camp_schedule_alert_conflict', FALSE);
  if ($alert_conflict) {
    foreach ($form_state['values']['nodes'] as $nid => $value) {
      if (is_numeric($nid)) {
        $room = $form_state['values']['nodes'][$nid]['field_session_room'][0]['value'];
        $day = $form_state['values']['nodes'][$nid]['field_session_day'][0]['value'];
        $time = $form_state['values']['nodes'][$nid]['field_session_time'][0]['value'];
        if ($room && $day && $time) {
          // Check if a session is already in thist spot.
          if ($values[$room][$day][$time]) {
            form_set_error('nodes]['. $nid .'][field_session_time', 'Scheduling conflict.');
          }
          $values[$room][$day][$time] = TRUE;
        }
      }
    }
  }
}

/**
 * Submit callback.
 */
function camp_schedule_manage_form_submit($form, &$form_state) {
  foreach ($form_state['values']['nodes'] as $nid => $value) {
    if (is_numeric($nid)) {
      $node = node_load($nid);
      $node->field_session_day[0]['value'] = $form_state['values']['nodes'][$nid]['field_session_day'][0]['value'];
      $node->field_session_time[0]['value'] = $form_state['values']['nodes'][$nid]['field_session_time'][0]['value'];
      $node->field_session_room[0]['value'] = $form_state['values']['nodes'][$nid]['field_session_room'][0]['value'];
      node_save($node);
    }
  }
}
