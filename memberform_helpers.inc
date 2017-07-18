<?php

/**
 * @file Helperfunctions for memberform module.
 */

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
