<?php

function memberform_shared_page_start_form(&$form, &$form_state) {
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
function memberform_shared_page_quickfinish_form(&$form, &$form_state) {
  $form['page_quickfinish_feedback'] = array(
    '#markup' => '<p>We nemen binnenkort contact met je op om het lidmaatschap af te ronden.</p><p>Ga verder als je zelf je inschrijving wilt afronden.</p>',
  );
}

/**
 * Form definition for person details page.
 */
function memberform_shared_page_person_details_form(&$form, &$form_state) {
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
 * Form definition for address page.
 */
function memberform_shared_page_address_form(&$form, &$form_state) {
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
function memberform_shared_page_contribution_form(&$form, &$form_state) {
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
function memberform_shared_page_gift_form(&$form, &$form_state, $gift_data) {
  $storage = $form_state['storage'];

  foreach ($gift_data as $gift_type => $gift_type_data) {
    // Show image.
    if (empty($storage['values']['gift'])) {
      $gift_key = $gift_type_data['default'];
    }
    else {
      $gift_key = $storage['values']['gift']['page_gift_select_' . $gift_type];
    }
    $image = $gift_type_data['gifts'][$gift_key]['image'];
    $form['page_gift_image_' . $gift_type . '_' . $gift_key] = array(
      '#markup' => '<div class="image"><img src="/' . drupal_get_path('module', 'memberform') . '/multipage_forms/gift_images/' . $image . '"/></div>',
    );
    // Show gift selection form.
    $options = array();
    foreach ($gift_type_data['gifts'] as $gift_key => $gift_data) {
      $options[$gift_key] = $gift_data['title'];
    }
    $form['page_gift_select_' . $gift_type] = array(
      '#title' => 'Welkomstgeschenk ' . $gift_type_data['title'],
      '#type' => 'radios',
      '#options' => $options,
      '#default_value' => $gift_type_data['default'],
      '#ajax' => array(
        'wrapper' => 'memberform_form_wrapper',
        'callback' => 'memberform_ajax_callback',
      ),
    );
  }
}

/**
 * Form definition for summary page.
 */
function memberform_shared_page_summary_form(&$form, &$form_state) {
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

  // Add extra data.
  if (!empty($storage['extra_data'])) {
    foreach ($storage['extra_data'] as $data) {
      $rows[] = array(
        'data' => array(
          0 => array(
            'data' => $data['title'],
            'header' => TRUE,
            'colspan' => 2,
          ),
        ),
        'no_striping' => TRUE,
      );
      foreach ($data['items'] as $rowdata) {
        $rows[] = array($rowdata['question']['display'], $rowdata['answer']['display']);
      }
    }
  }

  // Create table.
  $markup = theme('table', array('rows' => $rows));
  $form['overview'] = array(
    '#markup' => $markup,
  );
}

/**
 * Shared validation for the diffent shared form elements.
 */
function memberform_shared_validation($form, &$form_state) {
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
    }
  }
}
