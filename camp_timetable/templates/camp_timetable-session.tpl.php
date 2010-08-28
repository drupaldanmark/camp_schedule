<?php
// $Id: camp_timetable-session.tpl.php 191 2010-08-03 15:28:55Z mikkelhoegh $

/**
 * @file
 * Render a session within a slot on the timetable.
 */
?>
<div id="node-<?php print $node->nid; ?>" class="<?php print implode(' ', $classes); ?>">
  <h4><?php print l($node->title, 'node/' . $node->nid); ?></h4>
  <?php print $presenter_list; ?>
</div>

