<?php
// $Id: camp_timetable.pages.inc 214 2010-08-11 07:37:38Z beltofte $

/**
 * @file
 * Camp session timetable page callbacks.
 */

/**
 * Timesheet page callback.
 *
 * Will serve the timesheet from the cache, if available.
 */
function camp_timetable_pages_timetable() {
  $session_type = (arg(2) == 'bofs') ? 'bof' : 'session';
  
  // Add our stylesheets and javascript.
  $path = drupal_get_path('module', 'camp_timetable');
  drupal_add_css($path . '/css/camp_timetable.css');
  //drupal_add_js($path . '/js/camp_timetable.js');

  // Return data from cache if available.
  $cache = cache_get('camp_timetable_rendered_output_' . $session_type);
  if ($cache && !empty($cache->data)) {
    return $cache->data;
  }
  else {
    $output = _camp_timetable_pages_timetable($session_type);

    // Only cache output for english language with the default theme.
    if ($GLOBALS['language']->language == 'en' && $GLOBALS['theme_key'] == $GLOBALS['conf']['theme_default']) {
      cache_set('camp_timetable_rendered_output_' . $session_type, $output);
    }

    return $output;
  }
}

/**
 * Output the actual timetable.
 */
function _camp_timetable_pages_timetable($session_type = 'session') {
  // Variables used in the nested foreach.
  $sessions = camp_schedule_load_nodes(NULL, $session_type);
  $type = variable_get('camp_schedule_node_type', 'session');
  $fields = content_fields(NULL, $type);
  $room_labels = content_allowed_values($fields['field_session_room']);
  $day_labels = content_allowed_values($fields['field_session_day']);
  $time_labels = content_allowed_values($fields['field_session_time']);
  $hide_time_slots = array();
  if ($session_type == 'bof') {
    $hide_time_slots['20100824'] = array('11:00-12:30');
    $hide_time_slots['20100825'] = array('11:00-12:30');
    $hide_time_slots['20100826'] = array('11:00-12:30', '17:15-18:00');
  }  

  // Start the output with a wrapper div.
  // Imploding an array is quicker than repeated concatenation.
  $output = array('<div id="camp-timetable">');
  
  // Adding the help text
  $help_title = variable_get('camp_timetable_' . $session_type . '_help_title', '');
  $help_body = variable_get('camp_timetable_' . $session_type . '_help_body', '');
  $output[] = theme('camp_timetable_help', $help_title, $help_body);

  // Group nodes by day, time and room.
  $hierarchy = array();
  foreach ($sessions as $node) {
    $hierarchy[$node->field_session_day[0]['value']][$node->field_session_time[0]['value']][$node->field_session_room[0]['value']] = $node;
  }

  // Copenhagen-specific hack: Slicing off the session rooms for BOF's and the opposite for sessions.
  if ($session_type == 'bof') {
    $room_labels = array_slice($room_labels, 6);
  }
  else {
    $room_labels = array_slice($room_labels, 0, 6);
  }

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
        
      // Checking if the timeslot is hidden
      if (isset($hide_time_slots[$day_key]) && in_array($time_key, $hide_time_slots[$day_key])) {
        continue;
      }

      // The time table cell gets special treatment.
      $row[] = array(
        'class' => 'timeslot',
        'data' => $time_labels[$time_key],
        'header' => TRUE,
      );

      // Special case for keynotes.
      if ($time_key == '11:00-12:30') {
        $names = array(
          '20100824' => 'Dries Buytaert',
          '20100825' => 'Rasmus Lerdorf',
          '20100826' => 'Jeremy Keith',
        );

        $row[] = array(
          'class' => 'session-slot keynote-track',
          'data' => t('<h4>Keynote</h4><h4>@name</h4>', array('@name' => $names[$day_key])),
          'colspan' => 3,
        );

        $row[] = array(
          'class' => 'closed-slot',
          'data' => '',
          'colspan' => 3,
        );

        $rows[] = $row;
        continue;
      }
      elseif ($day_key == '20100826' && $time_key == '17:15-18:00') {
        $row[] = array(
          'class' => 'session-slot keynote',
          'data' => '<h4>Closing keynote</h4><h4>CPH team</h4>',
          'colspan' => 3,
        );

        $row[] = array(
          'class' => 'closed-slot',
          'data' => '',
          'colspan' => 3,
        );

        $rows[] = $row;
        continue;
      }

      // For each of the rooms, output a table cell.
      foreach (array_keys($room_labels) as $room_key) {
        $node = $hierarchy[$day_key][$time_key][$room_key];
        // If there are no sessions in a room at a specific slot,
        // output an empty cell.
        if (empty($node)) {
          $row[] = array(
            'class' => 'empty-slot',
            'data' => '',
          );
        }
        // Render each node in its cell.
        else {
          $classes = array('session-slot');

          // Add the track term to the classes.
          foreach ($node->taxonomy as $term) {
            // 23 is our track vocabulary.
            if ($term->vid == 23) {
              $classes[] = 'track-' . $term->tid;
            }
          }

          $row[] = array(
            'class' => implode(' ', $classes),
            'data' => theme('camp_timetable_session', $node),
          );
        }
      }

      $rows[] = $row;
    }

    // Render the full table.
    $output[] = theme('table', $header, $rows, array('class' => 'timetable'), $day_labels[$day_key]);
  }

  // Close the wrapper div.
  $output[] = '</div>';
  return implode("\n\n", $output);
}

