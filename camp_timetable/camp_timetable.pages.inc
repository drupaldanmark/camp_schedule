<?php
// $Id: camp_timetable.pages.inc 188 2010-08-03 13:41:16Z mikkelhoegh $

/**
 * @file
 * Camp session timetable page callbacks.
 */

/**
 * Output the actual timetable.
 */
function camp_timetable_pages_timetable() {
  // Add our stylesheets and javascript.
  $path = drupal_get_path('module', 'camp_timetable');
  drupal_add_css($path . '/css/camp_timetable.css');
  drupal_add_js($path . '/js/camp_timetable.js');

  // Variables used in the nested foreach.
  $sessions = camp_schedule_load_nodes();
  $type = variable_get('camp_schedule_node_type', 'session');
  $fields = content_fields(NULL, $type);
  $room_labels = content_allowed_values($fields['field_session_room']);
  $day_labels = content_allowed_values($fields['field_session_day']);
  $time_labels = content_allowed_values($fields['field_session_time']);
  $output = array();

  // Group nodes by day, time and room.
  $hierarchy = array();
  foreach ($sessions as $node) {
    $hierarchy[$node->field_session_day[0]['value']][$node->field_session_time[0]['value']][$node->field_session_room[0]['value']][] = $node;
  }

  // Copenhagen-specific hack: Slicing off the BOF rooms.
  $room_labels = array_slice($room_labels, 0, 6);

  // Use the room labels as table header.
  $header = $room_labels;

  // Add a header column for the starting time.
  array_unshift($header, t('Time'));

  // For each day, generate a time table.
  foreach (array_keys($day_labels) as $day_key) {
    $rows = array();

    // For each time slot, we make a table row.
    foreach (array_keys($time_labels) as $time_key) {
      $row = array();

      // The time table cell gets special treatment.
      $row[] = array(
        'class' => 'timeslot',
        'data' => $time_labels[$time_key],
        'header' => TRUE,
      );


      // For each of the rooms, output a table cell.
      foreach (array_keys($room_labels) as $room_key) {
        $sessions_here = $hierarchy[$day_key][$time_key][$room_key];
        // If there are no sessions in a room at a specific slot,
        // output an empty cell.
        if (empty($sessions_here)) {
          $row[] = array(
            'class' => 'empty-slot',
            'data' => '',
          );
        }
        else {
          $rendered = array(); 

          foreach ($sessions_here as $node) {
            $rendered[] = theme('camp_timetable_session', $node);
          }

          $row[] = array(
            'class' => 'session-room',
            'data' => implode("\n\n", $rendered),
          );
        }
      }

      $rows[] = $row;
    }

    $output[] = theme('table', $header, $rows, array('id' => 'camp-timetable'), $day_labels[$day_key]);
  }

  return implode("\n\n", $output);
}

