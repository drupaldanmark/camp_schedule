<?php
/**
 * @file
 * Render the help/info box in the top of the timetable page
 */
?>
<?php if (!empty($title) && !empty($body)): ?>
<h2 class="help-box-title"><?php print check_plain($title); ?></h2>
<div class="help-box-body">
  <?php print check_markup($body, NULL, FALSE); ?>
  <ul class="sessions-legend">
    <li class="track-7238">Code & Dev</li>
    <li class="track-7253">Services</li>
    <li class="track-7298">Intro</li>
    <li class="track-7303">Design</li>
    <li class="track-7308">Config</li>
    <li class="track-7313">Business</li>
  </ul>
  <div class="clearfix stupidpanels-csshideme">&nbsp;</div>
</div>
<?php endif; ?>