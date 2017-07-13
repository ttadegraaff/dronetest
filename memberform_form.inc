<?php

/**
 * @file
 * Memberform definition and validation and submit functions.
 */

/**
 * Form constructor for the member form.
 */
function memberform_form($form, &$form_state) {
  $last_page = 3;

  // Get storage from database if available.
  dpm($_SESSION);
  if (isset($_SESSION['memberform']['sid'])) {
    $sid = $_SESSION['memberform']['sid'];
    $query = "SELECT form_values FROM {memberform_storage} WHERE sid = :sid";
    $result = db_query($query, array(':sid' => $sid))->fetchAssoc();
    $form_state['storage'] = unserialize($result['form_values']);
    $form_state['storage']['sid'] = $sid;
  }

  // Set default page.
  if (!isset($form_state['storage']['page'])) {
    $form_state['storage']['page'] = 1;
  }

  $storage = $form_state['storage'];
  dpm($storage);

  // Create form based on form page.
  $form = array();

  $form['#prefix'] = '<div id="memberform_form_wrapper">';
  $form['#suffix'] = '</div>';

  $form['progress'] = array(
    '#markup' => '<h3>Page ' . $storage['page'] . '</h3>',
  );

  $form_callback = 'memberform_page_' . $storage['page'] . '_form';
  if (function_exists($form_callback)) {
    // Add page specific form elements.
    $form_callback($form);
    // Add default values.
    foreach ($form as $key => $item) {
      if (preg_match('/^page_' . $storage['page'] . '_[a-z_]+$/', $key, $matches)) {
        if (isset($storage['values'][$storage['page']][$key])) {
          $form[$key]['#default_value'] = $storage['values'][$storage['page']][$key];
        }
      }
    }
  }

  if ($storage['page'] > 1) {
    $form['previous_page'] = array(
      '#type' => 'submit',
      '#value' => 'Vorige',
      '#name' => 'previous_page',
      '#ajax' => array(
        'wrapper' => 'memberform_form_wrapper',
        'callback' => 'memberform_ajax_callback',
      ),
    );
  }

  if ($storage['page'] < $last_page) {
    $form['next_page'] = array(
      '#type' => 'submit',
      '#value' => 'Volgende',
      '#name' => 'next_page',
      '#ajax' => array(
        'wrapper' => 'memberform_form_wrapper',
        'callback' => 'memberform_ajax_callback',
      ),
    );
  }
  return $form;
}

/**
 * Validation for member form.
 */
function memberform_form_validate($form, &$form_state) {
  $values = $form_state['values'];
  $storage = $form_state['storage'];
  switch ($storage['page']) {
    case 1:
      if (empty($values['page_1_test'])) {
        form_set_error('page_' . $storage['page'] . '_test', 'Je kan niet verder zonder een vinkje!');
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
}

/**
 * Submit function for memberform.
 */
function memberform_form_submit($form, &$form_state) {
  $values = $form_state['values'];
  $storage = &$form_state['storage'];
  $sid = $storage['sid'];

  // Store values.
  $storage['values'][$storage['page']] = $values;

  // Process previous, next page.
  $trigger = $form_state['triggering_element']['#name'];

  switch ($trigger) {
    case 'previous_page':
      $storage['page']--;
      break;

    case 'next_page':
      $storage['page']++;
      break;
  }

  if (isset($storage['sid'])) {
    // Update database storage.
    db_update('memberform_storage')
      ->fields(array(
        'form_values' => serialize($storage),
      ))
      ->condition('sid', $storage['sid'])
      ->execute();
  }
  else {
    // Insert new database storage.
    $sid = db_insert('memberform_storage')
      ->fields(array(
      'hash' => md5(drupal_random_bytes(32)),
      'form_values' => serialize($storage),
      ))
      ->execute();
  }

  // Store submission id in session.
  $_SESSION['memberform']['sid'] = $sid;

  $form_state['rebuild'] = TRUE;
}

/**
 * Form definition for memberform page 1.
 */
function memberform_page_1_form(&$form) {
  $form['page_1_test'] = array(
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
 * Ajax callback function for memberform.
 */
function memberform_ajax_callback($form, &$form_state) {
  return $form;
}
