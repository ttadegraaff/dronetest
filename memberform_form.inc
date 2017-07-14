<?php

/**
 * @file
 * Memberform definition and validation and submit functions.
 */

/**
 * Form constructor for the member form.
 */
function memberform_form($form, &$form_state) {

  // Get storage from database if available.
  if (isset($_SESSION['memberform']['sid'])) {
    $sid = $_SESSION['memberform']['sid'];
    $query = "SELECT form_values FROM {memberform_storage} WHERE sid = :sid";
    $result = db_query($query, array(':sid' => $sid))->fetchAssoc();
    $storage = unserialize($result['form_values']);
    $storage['sid'] = $sid;
  }

  // Set default page.
  if (!isset($storage['page'])) {
    $storage['page'] = 1;
  }

  // Store storage data in form.
  $form_state['storage'] = $storage;

  // Create form based on form page.
  $form = array(
    '#validate' => array(
      'memberform_page_form_validate',
    ),
    '#submit' => array(
      'memberform_page_form_submit',
      'memberform_form_submit',
    ),
    '#prefix' => '<div id="memberform_form_wrapper">',
    '#suffix' => '</div>',
  );

  $form['progress'] = array(
    '#markup' => '<h3>Page ' . $storage['page'] . '</h3>',
  );

  // Get pages forms.
  $form_callback = 'memberform_page_' . $storage['page'] . '_form';
  if (function_exists($form_callback)) {
    // Add page specific form elements.
    $form_callback($form);
    // Add default values and error messages.
    foreach ($form as $key => $item) {
      if (preg_match('/^page_' . $storage['page'] . '_[a-z0-9_]+$/', $key, $matches)) {
        if (isset($storage['values'][$storage['page']][$key])) {
          $form[$key]['#default_value'] = $storage['values'][$storage['page']][$key];
        }
        if (isset($form_state['form_errors'][$key])) {
          $form[$key]['#prefix'] = theme('status_messages', drupal_set_message($form_state['form_errors'][$key], 'error'));
        }
      }
    }
  }

  // Add submit buttons.
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

  if ($storage['page'] < memberform_pagecount()) {
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

  if ($storage['page'] == memberform_pagecount()) {
    $form['finish'] = array(
      '#type' => 'submit',
      '#value' => 'Insturen',
      '#name' => 'finish',
    );
  }
  return $form;
}

/**
 * Submit function for memberform.
 */
function memberform_form_submit($form, &$form_state) {

  // Rebuild form when errors occured.
  if (isset($form_state['form_errors'])) {
    $form_state['rebuild'] = TRUE;
    return;
  }

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

  // Rebuild form when not finished.
  if ($trigger !== 'finish') {
    $form_state['rebuild'] = TRUE;
  }
  else {
    unset($_SESSION['memberform']['sid']);
  }
}

/**
 * Ajax callback function for memberform.
 */
function memberform_ajax_callback($form, &$form_state) {
  return $form;
}

/**
 * Removes error messages, and stores errors in form state.
 */
function memberform_remove_errors(&$form_state) {
  // Remove form error messages.
  if (!empty($form_state['form_errors'])) {
    $errors = $form_state['form_errors'];
    $error_messages = &$_SESSION['messages']['error'];
    if (!empty($error_messages)) {
      foreach ($errors as $error) {
        if (($key = array_search($error, $error_messages)) !== FALSE) {
          unset($error_messages[$key]);
          if (empty($error_messages)) {
            unset($_SESSION['messages']['error']);
          }
        }
      }
    }
    // Store errors for display in form elements.
    $form_state['form_errors'] = $errors;
    // Get the array of errors and reset them so form will be submitted.
    $errors = &drupal_static("form_set_error", array());
    // Make sure that each error should be applied to the field.
    foreach (array_keys($errors) as $field) {
      unset($errors[$field]);
    }
  }
}
