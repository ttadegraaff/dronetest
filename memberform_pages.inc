<?php

/**
 * @file Form definitions for memberform pages.
 *
 *   Define a function like 'memberform_page_x_form' where x is to be
 *   substituted with the page number to add a new form page.
 */

/**
 * Form settings.
 *
 * @param string $variable_name
 *   Name of the variable to return.
 *
 * @return Not defined
 *   Returns variable value.
 */
function memberform_settings($variable_name) {
  $settings = array(
    'test' => 'test',
  );
  return $settings[$variable_name];
}

/**
 * Form definition for memberform page 1.
 */
function memberform_page_1_form(&$form) {
  $form['page_1_test_1'] = array(
    '#type' => 'checkbox',
    '#title' => 'Zet een vinkje',
  );
  $form['page_1_test_2'] = array(
    '#type' => 'checkbox',
    '#title' => 'Zet een vinkje',
  );
}

/**
 * Form definition for memberform page 2.
 */
function memberform_page_2_form(&$form) {
  $form['page_2_test'] = array(
    '#type' => 'checkbox',
    '#title' => 'Zet een vinkje',
  );
}

/**
 * Form definition for memberform page 3.
 */
function memberform_page_3_form(&$form) {
  $form['page_3_test'] = array(
    '#type' => 'checkbox',
    '#title' => 'Zet een vinkje',
  );
}

/**
 * Validation for member form pages.
 */
function memberform_page_form_validate($form, &$form_state) {
  $values = $form_state['values'];
  $storage = $form_state['storage'];
  switch ($storage['page']) {
    case 1:
      if (empty($values['page_1_test_1'])) {
        form_set_error('page_1_test_1', 'Je kan niet verder zonder een vinkje!');
      }
      if (empty($values['page_1_test_2'])) {
        form_set_error('page_1_test_2', 'Je kan niet verder zonder een vinkje!');
      }
      break;

    case 2:
      if (empty($values['page_2_test'])) {
        form_set_error('page_2_test', 'Je kan niet verder zonder een vinkje!');
      }
      break;

    case 3:
      if (empty($values['page_3_test'])) {
        form_set_error('page_3_test', 'Je kan niet verder zonder een vinkje!');
      }
      break;
  }

  // Do no remove these lines.
  $form_state['form_errors'] = form_get_errors();
  memberform_remove_errors($form_state);
}

/**
 * Submit function for memberform pages.
 */
function memberform_page_form_submit($form, &$form_state) {
  $values = $form_state['values'];
  $storage = &$form_state['storage'];

  // Process trigger.
  $trigger = $form_state['triggering_element']['#name'];

  switch ($trigger) {
    case 'xxx':
      break;
  }
}
