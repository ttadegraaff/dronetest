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
 *   function memberform_membership_page_start_form(&$form, $storage, $values) {
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
function memberform_membership_settings($variable_name) {
  $settings = array(
    'path_name' => 'aanmelden/lid',
    'form_title' => 'Lid worden',
    'no_back_pages' => array(
      'quickfinish',
      'person_details',
    ),
    'titles' => array(
      'start' => 'Contactgegevens',
      'person_details' => 'Persoonsgegevens',
      'red' => 'ROOD lidmaatschap',
      'address' => 'Adresgegevens',
      'contribution' => 'Persoonlijke bijdrage',
      'gift' => 'Welkomstgeschenk',
      'summary' => 'Controleer uw gegevens',
    ),
  );
  return $settings[$variable_name];
}

/**
 * Form definition for start page.
 */
function memberform_membership_page_start_form(&$form, &$form_state) {
  memberform_shared_page_start_form($form, $form_state);
}

/**
 * Form definition for thanks phone page.
 */
function memberform_membership_page_quickfinish_form(&$form, &$form_state) {
  memberform_shared_page_quickfinish_form($form, $form_state);
}

/**
 * Form definition for person details page.
 */
function memberform_membership_page_person_details_form(&$form, &$form_state) {
  memberform_shared_page_person_details_form($form, $form_state);
  // Custom year range for membership and rood_membership forms.
  $form['page_person_details_birth_date']['#date_year_range'] = '-100:-15';
}

/**
 * Form definition for red membership page.
 */
function memberform_membership_page_red_form(&$form, &$form_state) {
  $storage = $form_state['storage'];

  $form['page_red_membership'] = array(
    '#title' => 'Rood lidmaatschap',
    '#type' => 'radios',
    '#options' => array(
      'sp_and_red_member' => 'Ja, ik wil ook lid worden van ROOD, Jong in de SP!',
      'sp_member' => 'Nee, ik wil alleen lid worden van de SP',
    ),
    '#required' => TRUE,
  );
}

/**
 * Form definition for address page.
 */
function memberform_membership_page_address_form(&$form, &$form_state) {
  memberform_shared_page_address_form($form, $form_state);
}

/**
 * Form definition for contribution page.
 */
function memberform_membership_page_contribution_form(&$form, &$form_state) {
  memberform_shared_page_contribution_form($form, $form_state);
}

/**
 * Form definition for gift page.
 *
 * Add images in module folder gift_images like book.jpg.
 */
function memberform_membership_page_gift_form(&$form, &$form_state) {

  $gifts = array(
    'sp' => array(
      'title' => 'SP',
      'default' => 'book',
      'gifts' => array(
        'book' => array(
          'title' => 'Boek Emile Roemer - Het kan wel',
          'image' => 'book.jpg',
        ),
        'usbstick' => array(
          'title' => 'SP usb stick 4G (Met SP informatie)',
          'image' => 'usbstick.jpg',
        ),
        'none' => array(
          'title' => 'Geen Welkomstgeschenk',
          'image' => 'none.jpg',
        ),
      ),
    ),
  );

  memberform_shared_page_gift_form($form, $form_state, $gifts);
}

/**
 * Form definition for summary page.
 */
function memberform_membership_page_summary_form(&$form, &$form_state) {
  memberform_shared_page_summary_form($form, $form_state);
}

/**
 * Form definition for thanks page.
 */
function memberform_membership_page_thanks_form(&$form, &$form_state) {
  $storage = $form_state['storage'];
  if (!empty($storage['values']['person_details']['page_person_details_first_name'])) {
    $name = $storage['values']['person_details']['page_person_details_first_name'];
  }
  else {
    $name = $storage['values']['start']['page_start_name'];
  }
  $form['thanks'] = array(
    '#markup' => '<p>Bedankt ' . check_plain($name) . ' voor je aanmelding als lid.</p><p>We gaan je inschrijving zo spoedig mogelijk verwerken. Je ontvangt binnenkort via de post je lidmaatschapskaart.</p>',
  );
}

/**
 * Validation for member form pages.
 */
function memberform_membership_page_form_validate($form, &$form_state) {
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
 * can be set: $storage['thanks_page'] to TRUE and the next page will have no
 * submit buttons.
 */
function memberform_membership_page_form_submit($form, &$form_state) {
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
          // Set form navigation.
          if (empty($values['page_start_phone'])) {
            $storage['next_page'] = 'person_details';
          }
          else {
            $storage['next_page'] = 'quickfinish';
          }
          break;

        case 'quickfinish':
          $storage['next_page'] = 'person_details';
          break;

        case 'person_details':
          $age = memberform_get_age($values['page_person_details_birth_date']);
          if ($age <= 27) {
            $storage['next_page'] = 'red';
          }
          else {
            $storage['next_page'] = 'address';
          }
          break;

        case 'red':
          $storage['membership_type'] = $values['page_red_membership'];
          $storage['next_page'] = 'address';
          break;

        case 'address':
          $storage['next_page'] = 'contribution';
          break;

        case 'contribution':
          $storage['next_page'] = 'gift';
          break;

        case 'gift':
          $storage['next_page'] = 'summary';
          $storage['finish_page'] = TRUE;
          break;

        case 'summary':
          $storage['next_page'] = 'thanks';
          $storage['thanks_page'] = TRUE;
      }
      break;
  }
}
