<?php

/**
 * @file
 * Memberform definition and validation and submit functions.
 */

/**
 * Form constructor for the member form.
 */
function memberform_form($form, &$form_state, $form_name) {
  $values = isset($form_state['values']) ? $form_state['values'] : array();
  $storage = memberform_get_storage(NULL, $form_name);

  // Store form name.
  $storage['form_name'] = $form_name;

  // Set default page.
  if (!isset($storage['current_page'])) {
    $storage['current_page'] = 'start';
    $storage['step'] = 1;
  }

  // Store storage data in form.
  $form_state['storage'] = $storage;
  $storage = &$form_state['storage'];

  if ($storage['form_name'] === 'intern') {
    $storage['finish_page'] = TRUE;
  }

  // Create form based on form page.
  $form = array(
    '#validate' => array(
      // This order is needed because of required elements inline message.
      'memberform_form_pre_custom_validate',
      'memberform_' . $form_name . '_page_form_validate',
      'memberform_form_post_custom_validate',
    ),
    '#submit' => array(
      'memberform_' . $form_name . '_page_form_submit',
      'memberform_form_submit',
    ),
    '#prefix' => '<div id="memberform_form_wrapper">',
    '#suffix' => '</div>',
    '#attached' => array(
      'css' => array(
        drupal_get_path('module', 'memberform') . '/css/memberform.css',
      ),
    ),
  );

  $file_path = drupal_get_path('module', 'memberform') . '/header_images/' . $storage['current_page'] . '.jpg';
  if (file_exists($file_path)) {
    $form['header'] = array(
      '#markup' => '<div class="form_header"><img src="/' . drupal_get_path('module', 'memberform') . '/header_images/' . $storage['current_page'] . '.jpg" /></div>',
    );
  }

  /*
   * Add form title and step.
   * $storage['step']
   * memberform_settings('titles')
   * $storage['current_page']
   */
  $settings_callback = 'memberform_' . $form_name . '_settings';
  $titles = $settings_callback('titles');
  if (!empty($titles[$storage['current_page']])) {
    $form['progress'] = array(
      '#markup' => '<h3>' . $titles[$storage['current_page']] . '</h3>',
    );
  }

  // Get pages forms.
  $form_callback = 'memberform_' . $form_name . '_page_' . $storage['current_page'] . '_form';
  if (function_exists($form_callback)) {

    // Add page specific form elements.
    $form_callback($form, $form_state);

    // Modify form elements.
    foreach ($form as $key => $item) {
      if (preg_match('/^page_' . $storage['current_page'] . '_[a-z0-9_]+$/', $key, $matches)) {
        // Add default values.
        if (isset($storage['values'][$storage['current_page']][$key])) {
          $form[$key]['#default_value'] = $storage['values'][$storage['current_page']][$key];
        }
        else {
          if (empty($form[$key]['#default_value'])) {
            $form[$key]['#default_value'] = '';
          }
        }
        // Add error messages.
        if (isset($form_state['form_errors'][$key])) {
          $form[$key]['#prefix'] = '<div class="memberform validation-error">';
          $form[$key]['#suffix'] = '<div class="messages error">' . $form_state['form_errors'][$key] . '</div></div>';
        }
        else {
          $form[$key]['#prefix'] = '<div class="memberform">';
          $form[$key]['#suffix'] = '</div>';
        }
        // Rebuild required functionality.
        if (!empty($form[$key]['#required'])) {
          unset($form[$key]['#required']);
          $form[$key]['#title'] .= ' <span class="form-required" title="This field is required.">*</span>';
          $storage['required_fields'][$key] = $key;
        }
        // Store form definition for use in other pages
        // (review page for example).
        $storage['form_element_definitions'][$key] = $form[$key];
      }
    }
  }

  // Add action buttons.
  if ($storage['step'] > 1) {
    $settings_callback = 'memberform_' . $form_name . '_settings';
    $no_back_pages = $settings_callback('no_back_pages');
    if (!in_array($storage['current_page'], $no_back_pages) && empty($storage['reset_page'])) {
      $form['previous_page'] = array(
        '#type' => 'submit',
        '#value' => 'Terug',
        '#name' => 'previous_page',
        '#ajax' => array(
          'wrapper' => 'memberform_form_wrapper',
          'callback' => 'memberform_ajax_callback',
        ),
      );
    }
  }

  if (empty($storage['finish_page'])) {
    $next_button_text = 'Verder';
    $next_button_name = 'next_page';
  }
  else {
    $next_button_text = 'Insturen';
    $next_button_name = 'finish_page';
  }
  if (empty($storage['reset_page']) || $storage['finish_page']) {
    $form['next_page'] = array(
      '#type' => 'submit',
      '#value' => $next_button_text,
      '#name' => $next_button_name,
      '#ajax' => array(
        'wrapper' => 'memberform_form_wrapper',
        'callback' => 'memberform_ajax_callback',
      ),
    );
  }

  // Clear form history when on final page.
  if (!empty($storage['reset_page'])) {
    memberform_clear_form($form_state);
  }

  return $form;
}

/**
 * Validate function for memberform.
 */
function memberform_form_pre_custom_validate($form, &$form_state) {
  $values = $form_state['values'];
  $storage = &$form_state['storage'];

  // Implement required fields validation.
  $trigger = $form_state['triggering_element']['#name'];
  if ($trigger !== 'previous_page') {
    foreach ($values as $key => $value) {
      if (isset($storage['required_fields'][$key])) {
        if (empty($value)) {
          form_set_error($key, $form[$key]['#title'] . ' is verplicht!');
        }
      }
    }
  }
}

/**
 * Validate function for memberform.
 */
function memberform_form_post_custom_validate($form, &$form_state) {
  $values = $form_state['values'];
  $storage = &$form_state['storage'];

  // Store values.
  foreach ($values as $key => $value) {
    if (!empty($value)) {
      $storage['values'][$storage['current_page']][$key] = $value;
    }
  }

  // Store data.
  memberform_store_form_data($storage);

  // Store submission id in session.
  $_SESSION['memberform'][$storage['form_name']]['sid'] = $storage['sid'];

  $form_state['form_errors'] = form_get_errors();
  memberform_remove_errors($form_state);
  unset($form_state['input']);
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
  unset($storage['values'][$storage['current_page']]);
  foreach ($values as $key => $value) {
    if (!empty($value)) {
      $storage['values'][$storage['current_page']][$key] = $value;
    }
  }

  // Process previous, next page.
  $trigger = $form_state['triggering_element']['#name'];

  switch ($trigger) {
    case 'previous_page':
      $storage['step']--;
      $storage['current_page'] = array_pop($storage['page_history']);
      unset($storage['next_page']);
      unset($storage['finish_page']);
      unset($storage['reset_page']);
      break;

    case 'next_page':
    case 'finish_page':
      $storage['step']++;
      $storage['page_history'][] = $storage['current_page'];
      $storage['current_page'] = $storage['next_page'];
      break;
  }

  // Store form data in database.
  memberform_store_form_data($storage);

  // Store submission id in session.
  $_SESSION['memberform'][$storage['form_name']]['sid'] = $storage['sid'];

  $form_state['rebuild'] = TRUE;
}

/**
 * Ajax callback function for memberform.
 */
function memberform_ajax_callback($form, &$form_state) {
  return $form;
}

function memberform_clear_form(&$form_state) {
  if (
    !empty($form_state['triggering_element']['#name']) &&
    in_array($form_state['triggering_element']['#name'], array('next_page', 'finish_page'))
  ) {
    unset($_SESSION['memberform']);
    unset($form_state['storage']);
    return TRUE;
  }
  return FALSE;
}
