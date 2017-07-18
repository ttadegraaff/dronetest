<?php

/**
 * @file Form definitions for memberform pages.
 *
 *   Define page form functions like 'memberform_page_x_form' where x is to be
 *   substituted with the page name used in $storage['current_page'] and
 *   $storage['next_page'].
 *
 *   Example:
 *
 *   function memberform_page_start_form(&$form, $storage, $values) {
 *     $form['page_start_choice'] = array(
 *       '#type' => 'radios',
 *       '#title' => 'Maak een keuze',
 *       '#options' => array('colors' => 'Kleuren', 'tastes' => 'Smaken'),
 *       '#required' => TRUE,
 *     );
 *   }
 */

/**
 * Form definition for memberform page start.
 */
function memberform_page_start_form(&$form, $storage, $values) {
  $form['page_start_choice'] = array(
    '#type' => 'radios',
    '#title' => 'Maak een keuze',
    '#options' => array('colors' => 'Kleuren', 'tastes' => 'Smaken'),
    '#required' => TRUE,
  );
}

/**
 * Form definition for memberform page 2.
 */
function memberform_page_colors_form(&$form, $storage, $values) {
  $form['page_colors_test'] = array(
    '#type' => 'radios',
    '#title' => 'Wat is je favoriete kleur?',
    '#options' => array('Groen', 'Rood', 'Blauw', 'Geel'),
    '#required' => TRUE,
  );
}

/**
 * Form definition for memberform page 3.
 */
function memberform_page_tastes_form(&$form, $storage, $values) {
  $form['page_tastes_test'] = array(
    '#type' => 'radios',
    '#title' => 'Wat is je favoriete smaak?',
    '#options' => array('Zuur', 'Zoet', 'Bitter', 'Zout'),
    '#required' => TRUE,
  );
}

/**
 * Validation for member form pages.
 */
function memberform_page_form_validate($form, &$form_state) {
  $values = $form_state['values'];
  $storage = $form_state['storage'];
  switch ($storage['current_page']) {
    case 'start':
      break;

    case 'colors':
      if (in_array($values['page_colors_test'], array(0, 1))) {
        form_set_error('page_colors_test', 'Nah.. dat kan echt niet!');
      }
      break;

    case 'tastes':
      break;
  }

  // Do no remove these lines.
  $form_state['form_errors'] = form_get_errors();
  memberform_remove_errors($form_state);
}

/**
 * Submit function for memberform pages.
 *
 * Set $storage['next_page'] to value of next page, and if next page is
 * the last one, set $storage['last_page'] to TRUE;
 */
function memberform_page_form_submit($form, &$form_state) {
  if (isset($form_state['form_errors'])) {
    return;
  }

  $values = $form_state['values'];
  $storage = &$form_state['storage'];

  // Process trigger.
  $trigger = $form_state['triggering_element']['#name'];

  switch ($trigger) {
    case 'next_page':
      switch ($storage['current_page']) {
        case 'start':
          $storage['next_page'] = $values['page_start_choice'];
          $storage['last_page'] = TRUE;
          break;

        case 'colors':
        case 'tastes':
          drupal_set_message('Finished!');
          break;
      }
      break;

    case 'finish':
      switch ($storage['current_page']) {
        case 'colors':
        case 'tastes':
          drupal_set_message('Finished!');
          break;
      }
      break;
  }
}
