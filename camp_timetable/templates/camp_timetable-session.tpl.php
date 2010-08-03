<?php
// $Id: camp_timetable-session.tpl.php 188 2010-08-03 13:41:16Z mikkelhoegh $

/**
 * @file
 * Render a session within a slot on the timetable.
 */
?>
<div class="<?php print implode(' ', $classes); ?>">
  <h4><?php print l($node->title, 'node/' . $node->nid); ?></h4>
  <?php print $presenter_list; ?>
</div>

