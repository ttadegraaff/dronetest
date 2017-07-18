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
 * Form settings.
 *
 * Add 'no_back_pages' variable (array) with page names of pages that should not
 * show a back button.
 *
 * @param string $variable_name
 *   Name of the variable to return.
 *
 * @return Variable value.
 *   Returns variable value.
 */
function memberform_settings($variable_name) {
  $settings = array(
    'no_back_pages' => array(
      //'thanks_phone',
      //'person_details',
    ),
  );
  return $settings[$variable_name];
}

/**
 * Form definition for start page.
 */
function memberform_page_start_form(&$form, $storage, $values) {
  $form['page_start_name'] = array(
    '#type' => 'textfield',
    '#title' => 'Naam',
  );

  $form['page_start_mail'] = array(
    '#type' => 'textfield',
    '#title' => 'E-mailadres',
  );

  $form['page_start_phone'] = array(
    '#type' => 'textfield',
    '#title' => 'Telefoonnummer',
  );
}

/**
 * Form definition for thanks phone page.
 */
function memberform_page_thanks_phone_form(&$form, $storage, $values) {
  $form['page_thanks_phone_feedback'] = array(
    '#markup' => '<p>Bedankt voor het aanmelden! We nemen binnenkort contact met je op om het lidmaatschap af te ronden.</p><p>Ga verder als je zelf je inschrijving wilt afronden.</p>',
  );
}

/**
 * Form definition for person details page.
 */
function memberform_page_person_details_form(&$form, $storage, $values) {
  $form['page_person_details_first_name'] = array(
    '#type' => 'textfield',
    '#title' => 'Voornaam',
    '#required' => TRUE,
  );

  $form['page_person_details_middle_name'] = array(
    '#type' => 'textfield',
    '#title' => 'Tussenvoegsel',
  );

  $form['page_person_details_last_name'] = array(
    '#type' => 'textfield',
    '#title' => 'Achternaam',
    '#required' => TRUE,
  );

  $form['page_person_details_birth_date'] = array(
    '#title'           => 'Geboortedatum',
    '#type'            => 'date_select',
    '#date_format'     => 'd-m-Y',
    '#date_year_range' => '-100:-15',
    '#required' => TRUE,
  );

}

/**
 * Validation for member form pages.
 */
function memberform_page_form_validate($form, &$form_state) {

  // Process trigger.
  $trigger = $form_state['triggering_element']['#name'];
  if ($trigger != 'previous_page') {

    $values = $form_state['values'];
    $storage = $form_state['storage'];

    switch ($storage['current_page']) {
      case 'start':
        drupal_set_message('Validated start');
        break;

      case 'thanks_phone':
        drupal_set_message('Validated thanks_phone');
        break;

      case 'person_details':
        drupal_set_message('Validated person_details');
        break;
    }
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
          if (empty($values['page_start_phone'])) {
            $storage['next_page'] = 'person_details';
          }
          else {
            $storage['next_page'] = 'thanks_phone';
          }
          break;
      }
      break;

    case 'finish':
      switch ($storage['current_page']) {
        case 'thanks_phone':
          break;

        case 'person_details':
          break;
      }
      break;
  }
}
