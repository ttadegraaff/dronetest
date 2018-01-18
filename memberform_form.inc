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

  $sid = empty($form_state['storage']['sid']) ? NULL : $form_state['storage']['sid'];
  $storage = memberform_get_storage($sid, $form_name);

  // Store storage data in form.
  $form_state['storage'] = $storage;
  $storage = &$form_state['storage'];

  // Store form name.
  $storage['form_name'] = $form_name;

  // Store settings.
  $settings_callback = 'memberform_' . $form_name . '_settings';
  $storage['pages'] = $settings_callback('pages');
  $storage['no_nav_pages'] = $settings_callback('no_nav_pages');
  $storage['final_submit_page'] = $settings_callback('final_submit_page');
  $storage['feedback_page'] = $settings_callback('feedback_page');


  // Set default page.
  if (empty($storage['current_page'])) {
    $storage['current_page'] = 'start';
    $storage['page_history'][] = 'start';
  }

  $form_state['validate'] = TRUE;


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

  // Add header image.
  $file_path = drupal_get_path('module', 'memberform') . '/header_images/' . $storage['current_page'] . '.jpg';
  if (file_exists($file_path)) {
    global $base_url;
    $form['header'] = array(
      '#markup' => '<div class="form_header"><img src="' . $base_url . '/' . drupal_get_path('module', 'memberform') . '/header_images/' . $storage['current_page'] . '.jpg" /></div>',
    );
  }

  // Add form title.
  if (!empty($storage['pages'][$storage['current_page']])) {
    $form['progress'] = array(
      '#markup' => '<h3>' . $storage['pages'][$storage['current_page']] . '</h3>',
    );
  }

  // Add form navigation.
  if ($storage['current_page'] !== $storage['feedback_page'] && !empty($storage['pages']) && count($storage['page_history']) > 1) {
    $form['form_navigation'] = array(
      '#type' => 'fieldset',
      '#attributes' => array('class' => array('memberform-navigation')),
    );
    $count = 0;
    foreach($storage['pages'] as $form_page => $title) {
      if (!in_array($form_page, $storage['no_nav_pages'])) {
        $count++;
        $storage['navigation_order'][$count] = $form_page;
        $form['form_navigation']['go_to_' . $form_page] = array(
          '#type' => 'submit',
          '#value' => $title,
          '#name' => 'go_to_' . $form_page,
          '#ajax' => array(
            'wrapper' => 'memberform_form_wrapper',
            'callback' => 'memberform_ajax_callback',
          ),
          '#disabled' => !(in_array($form_page, $storage['page_history']) || $form_page === $storage['current_page']),
          '#attributes' => ($form_page === $storage['current_page']) ? array('class' => array('active')) : array(),
        );
      }
    }
  }

  // Get pages forms.
  $form_callback = 'memberform_' . $form_name . '_page_' . $storage['current_page'] . '_form';
  if (function_exists($form_callback)) {

    // Add page specific form elements.
    $form_callback($form, $form_state);

    // Modify form elements (Add default values, error messages, rebuild required functionality).
    foreach ($form as $key => $item) {
      memberform_modify_form_elements($form[$key], $form_state, $key);
    }
  }

  // Add action buttons.
  if ($storage['current_page'] !== $storage['feedback_page'] && !empty($storage['previous_page']) && !in_array($storage['previous_page'], $storage['no_nav_pages'])) {
    $form['action']['previous_page'] = array(
      '#type' => 'submit',
      '#value' => 'Terug',
      '#name' => 'previous_page',
      '#ajax' => array(
        'wrapper' => 'memberform_form_wrapper',
        'callback' => 'memberform_ajax_callback',
      ),
    );
  }

  if ($storage['current_page'] !== $storage['final_submit_page']) {
    $next_button_text = 'Verder';
  }
  else {
    $next_button_text = 'Insturen';
  }
  if ($storage['current_page'] !== $storage['feedback_page'] && empty($form['action']['next_page'])) {
    $form['action']['next_page'] = array(
      '#type' => 'submit',
      '#value' => $next_button_text,
      '#name' => 'next_page',
      '#ajax' => array(
        'wrapper' => 'memberform_form_wrapper',
        'callback' => 'memberform_ajax_callback',
      ),
    );
  }
  if (!empty($form['action'])) {
    $form['action']['#type'] = 'fieldset';
  }

// Clear form history when on final page.
if ($storage['current_page'] === $storage['feedback_page']) {
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
  preg_match('/^go_to_([a-z_]+)$/', $trigger, $matches);
  if ($storage['current_page'] == end($storage['page_history']) && ($trigger == 'previous_page' || !empty($matches[1]))) {
    $form_state['validate'] = FALSE;
  }
  if ($form_state['validate']) {
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

  // Make sure form data is stored on ajax callback.
  if (empty($storage['sid'])) {
    $sid = db_insert('memberform_submissions')
    ->fields(array(
      'created' => REQUEST_TIME,
    ))
    ->execute();
    $storage['sid'] = $sid;
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

  $current_page_history_key = array_search($storage['current_page'], $storage['page_history']);
  // Process previous, next page.
  $trigger = $form_state['triggering_element']['#name'];

  switch ($trigger) {
    case 'previous_page':
      $storage['current_page'] = $storage['previous_page'];
      unset($storage['next_page']);
      break;

    case 'next_page':
      if (array_search($storage['current_page'], $storage['page_history']) !== FALSE) {
        $current_page_history_key = array_search($storage['current_page'], $storage['page_history']);
      }
      else {
        $current_page_history_key = FALSE;
      }
      $next_page_history_key = empty($storage['page_history'][$current_page_history_key + 1]) ? false : $current_page_history_key + 1;
      if ($next_page_history_key) {
        if ($storage['next_page'] === $storage['page_history'][$next_page_history_key]) {
          $storage['current_page'] = $storage['next_page'];
        }
        elseif (in_array($storage['next_page'], $storage['no_nav_pages'])) {
          $storage['current_page'] = $storage['page_history'][$next_page_history_key];
        }
        else {
          array_splice($storage['page_history'], $current_page_history_key + 1);
          $storage['current_page'] = $storage['next_page'];
        }
      }
      else {
        $storage['current_page'] = $storage['next_page'];
      }
      break;
  }

  // Process form navigation.
  preg_match('/^go_to_([a-z_]+)$/', $trigger, $matches);
  if (!empty($matches[1])) {
    $storage['current_page'] = $matches[1];
    unset($storage['next_page']);
  }

  // Add current page to history.
  if (array_search($storage['current_page'], $storage['page_history']) === FALSE && !in_array($storage['current_page'], $storage['no_nav_pages'])) {
    $storage['page_history'][] = $storage['current_page'];
  }

  // Set previous_page.
  $storage['previous_page'] = $storage['page_history'][array_search($storage['current_page'], $storage['page_history']) - 1];

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
  if (!empty($form_state['storage']['sid'])) {
    $number = db_delete('memberform_storage')
    ->condition('sid', $form_state['storage']['sid'])
    ->execute();
  }
  unset($_SESSION['memberform']);
  unset($form_state['storage']);
}

function memberform_modify_form_elements(&$item, &$form_state, $key) {
  $item_org = $item;
  $storage = &$form_state['storage'];
  if (is_array($item)) {
    if (preg_match('/^page_' . $storage['current_page'] . '_[a-z0-9_]+$/', $key, $matches)) {
      // Add default values.
      if (isset($storage['values'][$storage['current_page']][$key])) {
        $item['#default_value'] = $storage['values'][$storage['current_page']][$key];
      }
      else {
        if (empty($item['#default_value'])) {
          $item['#default_value'] = '';
        }
      }
      // Add error messages.
      if (isset($form_state['form_errors'][$key])) {
        $item['#prefix'] = '<div class="memberform validation-error">';
        $item['#suffix'] = '<div class="messages error">' . $form_state['form_errors'][$key] . '</div></div>';
      }
      else {
        $item['#prefix'] = '<div class="memberform">';
        $item['#suffix'] = '</div>';
      }
      // Rebuild required functionality.
      if (!empty($item['#required'])) {
        unset($item['#required']);
        $item['#title'] .= ' <span class="form-required" title="This field is required.">*</span>';
        $storage['required_fields'][$key] = $key;
      }
      // Store form definition for use in other pages
      // (review page for example).
      $storage['form_element_definitions'][$key] = $item;
    }
    else {
      foreach ($item as $key2 => $item2) {
        memberform_modify_form_elements($item[$key2], $form_state, $key2);
      }
    }
  }
}
