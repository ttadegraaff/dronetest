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
 *   function memberform_rood_membership_page_start_form(&$form, $storage, $values) {
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
function memberform_rood_membership_settings($variable_name) {
  $settings = array(
    'path_name' => 'aanmelden/rood',
    'form_title' => 'ROOD lid worden',
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
function memberform_rood_membership_page_start_form(&$form, &$form_state) {
  $form['page_start_name'] = array(
    '#type' => 'textfield',
    '#title' => 'Naam',
    '#required' => TRUE,
  );

  $form['page_start_email'] = array(
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
function memberform_rood_membership_page_quickfinish_form(&$form, &$form_state) {
  $form['page_quickfinish_feedback'] = array(
    '#markup' => '<p>We nemen binnenkort contact met je op om het lidmaatschap af te ronden.</p><p>Ga verder als je zelf je inschrijving wilt afronden.</p>',
  );
}

/**
 * Form definition for person details page.
 */
function memberform_rood_membership_page_person_details_form(&$form, &$form_state) {
  $storage = &$form_state['storage'];

  // Split name given in start page.
  $parts = array('first_name', 'middle_name', 'last_name');
  if (empty($storage['values']['person_details']['page_person_details_last_name'])) {
    // Prefill name parts.
    $split_name = memberform_split_name($storage['values']['start']['page_start_name']);
    foreach ($parts as $part) {
      $$part = isset($split_name[$part]) ? $split_name[$part] : '';
      $storage['values']['person_details']['page_person_details_' . $part] = $$part;
    }
  }
  else {
    // Show stored name parts.
    foreach ($parts as $part) {
      $$part = isset($storage['values']['person_details']['page_person_details_' . $part]) ? $storage['values']['person_details']['page_person_details_' . $part] : '';
    }
  }

  $form['page_person_details_first_name'] = array(
    '#type' => 'textfield',
    '#title' => 'Voornaam',
    '#required' => TRUE,
    '#default_value' => $first_name,
  );

  $form['page_person_details_middle_name'] = array(
    '#type' => 'textfield',
    '#title' => 'Tussenvoegsel',
    '#default_value' => $middle_name,
  );

  $form['page_person_details_last_name'] = array(
    '#type' => 'textfield',
    '#title' => 'Achternaam',
    '#required' => TRUE,
    '#default_value' => $last_name,
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
 * Form definition for red membership page.
 */
function memberform_rood_membership_page_red_form(&$form, &$form_state) {
  $storage = $form_state['storage'];

  $form['page_red_membership'] = array(
    '#title' => 'Rood lidmaatschap',
    '#type' => 'radios',
    '#options' => array(
      'rood_member' => 'Ja, ik wil ook lid worden van ROOD, Jong in de SP!',
      'no_rood_member' => 'Nee, ik wil alleen lid worden van de SP',
    ),
    '#required' => TRUE,
  );
}

/**
 * Form definition for address page.
 */
function memberform_rood_membership_page_address_form(&$form, &$form_state) {
  $storage = $form_state['storage'];

  include_once DRUPAL_ROOT . '/includes/locale.inc';
  $countries = country_get_list();
  $form['page_address_country'] = array(
    '#title'         => 'Land',
    '#type'          => 'select',
    '#options'       => $countries,
    '#default_value' => 'NL',
    '#required'      => TRUE,
    '#ajax' => array(
      'wrapper' => 'memberform_form_wrapper',
      'callback' => 'memberform_ajax_callback',
    ),
  );

  if (
    empty($storage['values']['address']['page_address_country']) ||
    $storage['values']['address']['page_address_country'] === 'NL'
  ) {
    // Dutch address, autofill street and locality fields.
    $form['page_address_postal_code'] = array(
      '#title'         => 'Postcode',
      '#type'          => 'textfield',
      '#required'      => TRUE,
      '#ajax' => array(
        'wrapper' => 'memberform_form_wrapper',
        'callback' => 'memberform_ajax_callback',
      ),
    );
    $form['page_address_house_number'] = array(
      '#title'         => 'Huisnummer',
      '#type'          => 'textfield',
      '#required'      => TRUE,
      '#ajax' => array(
        'wrapper' => 'memberform_form_wrapper',
        'callback' => 'memberform_ajax_callback',
      ),
    );
    $form['page_address_house_number_addition'] = array(
      '#title'         => 'Huisnummer toevoeging',
      '#type'          => 'textfield',
    );
    // Get address data based on postal code and house number.
    if (!empty($storage['values']['address']['page_address_postal_code'])) {
      $house_number = (isset($storage['values']['address']['page_address_house_number'])) ? $storage['values']['address']['page_address_house_number'] : NULL;
      $address_data = memberform_postcode_lookup($storage['values']['address']['page_address_postal_code'], $house_number);
      if (!is_object($address_data) && !empty($address_data)) {
        // Show warning when postalcode housenumber combination is not found.
        $message = '<div class="messages warning">' . $address_data . '</div>';
        $form['page_address_house_number']['#field_suffix'] = empty($form['page_address_house_number']['#field_suffix']) ? $message : $form['page_address_house_number']['#field_suffix'] . $message;
      }
    }
    // Set street value.
    $form['page_address_street'] = array(
      '#title'         => 'Straat',
      '#type'          => 'textfield',
      '#required'      => TRUE,
    );
    // Set locality value.
    $form['page_address_locality'] = array(
      '#title'         => 'Plaats',
      '#type'          => 'textfield',
      '#required'      => TRUE,
    );
    if (!empty($address_data->adres)) {
      $form['page_address_street']['#default_value'] = $address_data->adres;
    };
    if (!empty($address_data->woonplaats)) {
      $form['page_address_locality']['#default_value'] = $address_data->woonplaats;
    };
  }
  else {
    // International address.
    $form['page_address_street'] = array(
      '#title'         => 'Straat',
      '#type'          => 'textfield',
      '#required'      => TRUE,
    );
    $form['page_address_house_number'] = array(
      '#title'         => 'Huisnummer',
      '#type'          => 'textfield',
      '#required'      => TRUE,
    );
    $form['page_address_house_number_addition'] = array(
      '#title'         => 'Huisnummer toevoeging',
      '#type'          => 'textfield',
    );
    $form['page_address_postal_code'] = array(
      '#title'         => 'Postcode',
      '#type'          => 'textfield',
    );
    $form['page_address_locality'] = array(
      '#title'         => 'Plaats',
      '#type'          => 'textfield',
      '#required'      => TRUE,
    );
  }
}

/**
 * Form definition for contribution page.
 */
function memberform_rood_membership_page_contribution_form(&$form, &$form_state) {
  $storage = $form_state['storage'];
  $form['page_contribution_choice'] = array(
    '#title' => 'Kwartaalbijdrage',
    '#type' => 'radios',
    '#options' => array(
      '50' => '50,-',
      '20' => '20,-',
      '10' => '10,-',
      'custom' => 'Anders',
    ),
    '#required' => TRUE,
    '#ajax' => array(
      'wrapper' => 'memberform_form_wrapper',
      'callback' => 'memberform_ajax_callback',
    ),
  );

  if ($storage['values']['contribution']['page_contribution_choice'] === 'custom') {
    $form['page_contribution_custom'] = array(
      '#title' => 'Kies een bedrag (minimaal 5 euro)',
      '#type' => 'textfield',
      '#required' => TRUE,
      '#field_prefix' => '<div class="inline">€</div>',
    );
  }

  $form['page_contribution_payment_type'] = array(
    '#title' => 'Hoe wil je je bijdrage voldoen?',
    '#type' => 'radios',
    '#required' => TRUE,
    '#options' => array(
      'auto' => 'Automatische incasso',
      'accept' => 'Acceptgiro',
    ),
    '#name' => 'page_contribution_payment_type',
    '#ajax' => array(
      'wrapper' => 'memberform_form_wrapper',
      'callback' => 'memberform_ajax_callback',
    ),
  );

  $form['page_contribution_iban'] = array(
    '#title' => 'Wat is je bankrekeningnummer?',
    '#type' => 'textfield',
    '#states' => array(
      'visible' => array(
        ':input[name="page_contribution_payment_type"]' => array(
          'value' => 'auto',
        ),
      ),
    ),
  );
}

/**
 * Form definition for gift page.
 *
 * Add images in module folder gift_images like book.jpg.
 */
function memberform_rood_membership_page_gift_form(&$form, &$form_state) {
  $storage = $form_state['storage'];

  $gifts = array(
    'book' => 'Boek Emile Roemer - Het kan wel',
    'usbstick' => 'SP usb stick 4G (Met SP informatie)',
    'none' => 'Geen Welkomstgeschenk',
  );

  foreach ($gifts as $key => $title) {
    if (empty($storage['values']['gift'])) {
      reset($gifts);
      $key = key($gifts);
      $form['page_gift_' . $key] = array(
        '#markup' => '<div class="image"><img src="' . drupal_get_path('module', 'memberform') . '/gift_images/' . $key . '.jpg" /></div>',
      );
    }
    else {
      if ($storage['values']['gift']['page_gift_select'] === $key) {
        $form['page_gift_' . $key] = array(
          '#markup' => '<div class="image"><img src="' . drupal_get_path('module', 'memberform') . '/gift_images/' . $key . '.jpg" /></div>',
        );
      }
    }
  }

  $form['page_gift_select'] = array(
    '#type' => 'radios',
    '#options' => $gifts,
    '#default_value' => 'book',
    '#ajax' => array(
      'wrapper' => 'memberform_form_wrapper',
      'callback' => 'memberform_ajax_callback',
    ),
  );
}

/**
 * Form definition for summary page.
 */
function memberform_rood_membership_page_summary_form(&$form, &$form_state) {
  $storage = $form_state['storage'];
  $values = $storage['values'];
  $page_titles = memberform_membership_settings('titles');

  $markup = '';
  $rows = array();

  // Loop through pages.
  foreach ($values as $page => $items) {
    if (in_array($page, $storage['page_history'])) {
      $page_title = (empty($page_titles[$page])) ? '' : $page_titles[$page];
      $rows_tmp[] = array(
        'data' => array(
          0 => array(
            'data' => $page_title,
            'header' => TRUE,
            'colspan' => 2,
          ),
        ),
        'no_striping' => TRUE,
      );
      // Loop through form elements.
      foreach ($items as $key => $value) {
        if (!empty($storage['form_element_definitions'][$key])) {
          $form_element = $storage['form_element_definitions'][$key];
          $title = (empty($form_element['#title'])) ? '' : $form_element['#title'];
          if (strpos($key, 'page_') === 0) {
            // Parse output of different form element types.
            switch ($form_element['#type']) {
              case 'textfield':
                $rows_tmp[] = array($title, $value);
                break;

              case 'date_select':
                $date = date("d-m-Y", strtotime($value));
                $rows_tmp[] = array($title, $date);
                break;

              case 'select':
              case 'radios':
                $display = $form_element['#options'][$value];
                $rows_tmp[] = array($title, $display);
                break;
            }
          }
        }
      }
      // Do not output pages without interactive elements.
      if (count($rows_tmp) > 1) {
        $rows = array_merge($rows, $rows_tmp);
      }
      $rows_tmp = array();
    }
  }
  // Create table.
  $markup = theme('table', array('rows' => $rows));
  $form['overview'] = array(
    '#markup' => $markup,
  );
}

/**
 * Form definition for thanks page.
 */
function memberform_rood_membership_page_thanks_form(&$form, &$form_state) {
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
function memberform_rood_membership_page_form_validate($form, &$form_state) {
  $values = &$form_state['values'];
  $storage = &$form_state['storage'];

  // Process trigger.
  $trigger = $form_state['triggering_element']['#name'];
  if ($trigger != 'previous_page') {

    switch ($storage['current_page']) {
      case 'start':
        if (!empty($values['page_start_email'])) {
          if (!valid_email_address($values['page_start_email'])) {
            form_set_error('page_start_email', 'Geef een geldig e-mailadres op!');
          }
        }
        if (empty($values['page_start_email']) && empty($values['page_start_phone'])) {
          form_set_error('page_start_phone', 'Geef tenminste een e-mailadres of telefoonnummer op zodat we je kunnen bereiken.');
        }
        break;

      case 'quickfinish':
        break;

      case 'person_details':
        break;

      case 'address':
        $checks = array(
          'postal_code' => array(
            'regex' => '/^[1-9][0-9]{3}\s?[a-zA-Z]{2}$/',
            'message' => 'Vul een correcte postcode in',
          ),
          'house_number' => array(
            'regex' => '/^[1-9][0-9]*$/',
            'message' => 'Vul een huisnummer in',
          ),
        );
        // Do no check International postal codes.
        if ($values['page_address_country'] !== 'NL') {
          unset($checks['postal_code']);
        }
        foreach ($checks as $key => $check) {
          if (!empty($values['page_address_' . $key])) {
            if (!preg_match($check['regex'], $values['page_address_' . $key])) {
              form_set_error('page_address_' . $key, $check['message']);
            }
          }
        }
        break;

      case 'contribution':
        if ($values['page_contribution_choice'] === 'custom') {
          unset($values['amount']);
          $amount = $values['page_contribution_custom'];
          $amount = preg_replace('/[^0-9,.]/', '', $amount);
          $amount = str_replace(',', '.', $amount);
          if (!is_numeric($amount)) {
            form_set_error('page_contribution_custom', 'Vul een getal in.');
          }
          else {
            $amount = number_format($amount, 2, '.', '');
            if ($amount < 5) {
              form_set_error('page_contribution_custom', 'De minimumbijdrage is 5 euro.');
            }
            $amount = str_replace('.', ',', $amount);
            $values['page_contribution_custom'] = $amount;
          }
        }
        if ($values['page_contribution_payment_type'] === 'auto') {
          $iban = strtoupper($values['page_contribution_iban']);
          $iban = preg_replace('/[^0-9A-Z]/', '', $iban);
          if ($storage['values']['address']['page_address_country'] === 'NL') {
            if (!memberform_check_iban($iban)) {
              form_set_error('page_contribution_iban', 'Vul een geldig iban nummer in.');
            }
            else {
              $values['page_contribution_iban'] = $iban;
            }
          }
        }
        break;

      case 'summary':
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
function memberform_rood_membership_page_form_submit($form, &$form_state) {
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
          $birthdate = new DateTime($values['page_person_details_birth_date']);
          $today   = new DateTime('today');
          $age = $birthdate->diff($today)->y;
          if ($age <= 27) {
            $storage['next_page'] = 'red';
          }
          else {
            $storage['next_page'] = 'address';
          }
          break;

        case 'red':
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
