<?php

/**
 * File for admin functions.
 */

/**
 * Admin settings form.
 * 
 * @param array $form_state
 */
function camp_timetable_settings($form_state) {
  $form['sessions'] = array(
    '#type' => 'fieldset',
    '#title' => t('Sessions'),
    '#description' => '',
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['sessions']['camp_timetable_session_help_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Help title'),
    '#description' => t('Title used in the help box on the session timetable page.'),
    '#default_value' => variable_get('camp_timetable_session_help_title', ''),
  );
  $form['sessions']['camp_timetable_session_help_body'] = array(
    '#type' => 'textarea',
    '#title' => t('Help body'),
    '#description' => t('Body used in the help box on the session timetable page.'),
    '#default_value' => variable_get('camp_timetable_session_help_body', ''),
  );
  
  $form['bof'] = array(
    '#type' => 'fieldset',
    '#title' => t("BOF's"),
    '#description' => '',
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['bof']['camp_timetable_bof_help_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Help title'),
    '#description' => t('Title used in the help box on the BOF timetable page.'),
    '#default_value' => variable_get('camp_timetable_bof_help_title', ''),
  );
  $form['bof']['camp_timetable_bof_help_body'] = array(
    '#type' => 'textarea',
    '#title' => t('Help body'),
    '#description' => t('Body used in the help box on the BOF timetable page.'),
    '#default_value' => variable_get('camp_timetable_bof_help_body', ''),
  );
  return system_settings_form($form); 
}
