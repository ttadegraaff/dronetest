<?php

/**
 * @file Form definitions for memberform pages.
 *
 *   Define page form functions like 'memberform_x_page_x_form' where the
 *   first x is to be substituted with the form name and the second x with the
 *   page name used in $storage['current_page'] and $storage['next_page'].
 *
 *   Example:
 *
 *   function memberform_tribune_page_start_form(&$form, $storage, $values) {
 *     $form['page_start_choice'] = array(
 *       '#type' => 'radios',
 *       '#title' => 'Maak een keuze',
 *       '#options' => array('colors' => 'Kleuren', 'tastes' => 'Smaken'),
 *       '#required' => TRUE,
 *     );
 *   }
 */

/**
 * Form settings.
 *
 * Add 'no_back_pages' variable (array) with page names of pages that should not
 * show a back button.
 *
 * Add 'titles' array with page name to page titles.
 *
 * @param string $variable_name
 *   Name of the variable to return.
 *
 * @return Variable value.
 *   Returns variable value.
 */
function memberform_tribune_settings($variable_name) {
  $settings = array(
    'path_name' => 'aanmelden/tribune',
    'form_title' => 'Aanmelden tribune',
    'no_back_pages' => array(
      'quickfinish',
      'person_details',
    ),
    'titles' => array(
      'start' => 'Contactgegevens',
      'person_details' => 'Persoonsgegevens',
      'address' => 'Adresgegevens',
      'payment' => 'Betalingsgegevens',
      'summary' => 'Controleer uw gegevens',
    ),
  );
  return $settings[$variable_name];
}

/**
 * Form definition for start page.
 */
function memberform_tribune_page_start_form(&$form, &$form_state) {
  memberform_shared_page_start_form($form, $form_state);
}

/**
 * Form definition for thanks phone page.
 */
function memberform_tribune_page_quickfinish_form(&$form, &$form_state) {
  memberform_shared_page_quickfinish_form($form, $form_state);
}

/**
 * Form definition for person details page.
 */
function memberform_tribune_page_person_details_form(&$form, &$form_state) {
  memberform_shared_page_person_details_form($form, $form_state);
  // Do no ask birth date.
  unset($form['page_person_details_birth_date']);
  // Ask for SP membership.
  $form['page_person_details_spmember'] = array(
    '#type' => 'checkbox',
    '#title' => 'Ik ben lid van de SP',
  );
}


/**
 * Form definition for address page.
 */
function memberform_tribune_page_address_form(&$form, &$form_state) {
  memberform_shared_page_address_form($form, $form_state);
}

/**
 * Form definition for contribution page.
 */
function memberform_tribune_page_payment_form(&$form, &$form_state) {
  memberform_shared_page_payment_form($form, $form_state);
}

/**
 * Form definition for summary page.
 */
function memberform_tribune_page_summary_form(&$form, &$form_state) {
  memberform_shared_page_summary_form($form, $form_state);
}

/**
 * Form definition for thanks page.
 */
function memberform_tribune_page_thanks_form(&$form, &$form_state) {
  $storage = $form_state['storage'];
  if (!empty($storage['values']['person_details']['page_person_details_first_name'])) {
    $name = $storage['values']['person_details']['page_person_details_first_name'];
  }
  else {
    $name = $storage['values']['start']['page_start_name'];
  }
  $form['thanks'] = array(
    '#markup' => '<p>Bedankt ' . check_plain($name) . '.</p><p>We gaan je aanmelding zo spoedig mogelijk verwerken.',
  );
}

/**
 * Validation for member form pages.
 */
function memberform_tribune_page_form_validate($form, &$form_state) {
  memberform_shared_validation($form, $form_state);

  $values = &$form_state['values'];
  $storage = &$form_state['storage'];

  // Process trigger.
  $trigger = $form_state['triggering_element']['#name'];
  if ($trigger != 'previous_page') {

    switch ($storage['current_page']) {
      case 'test':
        break;
    }
  }
}

/**
 * Submit function for memberform pages.
 *
 * Set $storage['next_page'] to value of next page. The text of the submit
 * button can be set to 'submit' instead of 'next' by setting
 * $storage['finish_page'] to TRUE. Next a thanks page without submit buttons
 * can be set: $storage['reset_page'] to TRUE and the next page will have no
 * submit buttons.
 */
function memberform_tribune_page_form_submit($form, &$form_state) {
  $values = $form_state['values'];
  $storage = &$form_state['storage'];

  // Process trigger.
  $trigger = $form_state['triggering_element']['#name'];

  // Store membership.
  switch ($trigger) {
    case 'next_page':
      switch ($storage['current_page']) {
        case 'start':
          // Store membership type.
          $storage['membership_type'] = 'tribune';
          break;
      }
      break;
  }

  memberform_shared_submit($form, $form_state);

  // Override shared submit.
  switch ($trigger) {
    case 'next_page':
      switch ($storage['current_page']) {
        case 'address':
          $storage['next_page'] = 'payment';
          break;
      }
      break;
  }
}