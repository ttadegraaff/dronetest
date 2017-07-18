<?php

/**
 * @file
 * Memberform definition and validation and submit functions.
 */

/**
 * Form constructor for the member form.
 */
function memberform_form($form, &$form_state) {
  $values = isset($form_state['values']) ? $form_state['values'] : array();
  $storage = memberform_get_storage();

  // Set default page.
  if (!isset($storage['current_page'])) {
    $storage['current_page'] = 'start';
    $storage['step'] = 1;
  }

  // Store storage data in form.
  $form_state['storage'] = $storage;
  dpm($storage);

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
    '#markup' => '<h3>Stap ' . $storage['step'] . '</h3>',
  );

  // Get pages forms.
  $form_callback = 'memberform_page_' . $storage['current_page'] . '_form';
  if (function_exists($form_callback)) {

    // Add page specific form elements.
    $form_callback($form, $storage, $values);

    // Modify form elements.
    foreach ($form as $key => $item) {
      if (preg_match('/^page_' . $storage['current_page'] . '_[a-z0-9_]+$/', $key, $matches)) {
        // Add default values.
        if (isset($storage['values'][$storage['current_page']][$key])) {
          $form[$key]['#default_value'] = $storage['values'][$storage['current_page']][$key];
        }
        // Add error messages.
        if (isset($form_state['form_errors'][$key])) {
          $form[$key]['#prefix'] = '<div class="memberform validation-error"><div class="messages error">' . $form_state['form_errors'][$key] . '</div>';
        }
        else {
          $form[$key]['#prefix'] = '<div class="memberform">';
        }
        $form[$key]['#suffix'] = '</div>';
      }
    }
  }

  // Add action buttons.
  if ($storage['step'] > 1) {
    $no_back_pages = memberform_settings('no_back_pages');
    if (!in_array($storage['current_page'], $no_back_pages)) {
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
  }

  if (!empty($storage['last_page'])) {
    $form['finish'] = array(
      '#type' => 'submit',
      '#value' => 'Insturen',
      '#name' => 'finish',
    );
  }
  else {
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

  // Store values.
  $storage['values'][$storage['current_page']] = $values;

  // Process previous, next page.
  $trigger = $form_state['triggering_element']['#name'];

  switch ($trigger) {
    case 'previous_page':
      $storage['step']--;
      $storage['current_page'] = array_pop($storage['page_history']);
      unset($storage['last_page']);
      unset($storage['next_page']);
      break;

    case 'next_page':
      $storage['step']++;
      $storage['page_history'][] = $storage['current_page'];
      $storage['current_page'] = $storage['next_page'];
      break;
  }

  // Store form data in database.
  memberform_store_form_data($storage);

  // Store submission id in session.
  $_SESSION['memberform']['sid'] = $storage['sid'];

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

/**
 * Gets stored form data from database.
 *
 * @param int $sid
 *   Form submission id.
 *
 * @return array $storage
 *   Stored form data.
 */
function memberform_get_storage($sid = NULL) {
  $storage = array();
  if (!isset($sid) && isset($_SESSION['memberform']['sid'])) {
    $sid = $_SESSION['memberform']['sid'];
  }

  if (isset($sid)) {
    // Get storage from database if available.
    $query = "SELECT form_values FROM {memberform_storage} WHERE sid = :sid";
    $result = db_query($query, array(':sid' => $sid))->fetchAssoc();
    $storage = unserialize($result['form_values']);
    $storage['sid'] = $sid;
  }
  return $storage;
}

/**
 * Stores database storage variable in database.
 *
 * @param array $storage
 *   Database storage variable.
 */
function memberform_store_form_data(array &$storage = NULL) {
  if (isset($storage)) {
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
      $storage['sid'] = db_insert('memberform_storage')
        ->fields(array(
          'hash' => md5(drupal_random_bytes(32)),
          'form_values' => serialize($storage),
        ))
        ->execute();
    }
  }
}
