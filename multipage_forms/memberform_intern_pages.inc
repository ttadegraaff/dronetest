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
 *   function memberform_intern_page_start_form(&$form, $storage, $values) {
 *     $form['input']['page_start_choice'] = array(
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
function memberform_intern_settings($variable_name) {
  $settings = array(
    'path_name' => 'aanmelden/intern',
    'form_title' => 'Aanmelden intern',
    'no_back_pages' => array(),
    'titles' => array(
      'start' => '',
      'summary' => 'Controleer gegevens',
    ),
  );
  return $settings[$variable_name];
}

/**
 * Form definition for start page.
 */
function memberform_intern_page_start_form(&$form, &$form_state) {
  $storage = &$form_state['storage'];
  $values = &$storage['values'];

  $form['input_person_details'] = array(
    '#type' => 'fieldset',
    '#title' => 'Persoonsgegevens',
  );

  $form['input_person_details']['page_start_first_name'] = array(
    '#type' => 'textfield',
    '#title' => 'Voornaam',
    '#required' => TRUE,
    '#default_value' => $first_name,
  );

  $form['input_person_details']['page_start_middle_name'] = array(
    '#type' => 'textfield',
    '#title' => 'Tussenvoegsel',
    '#default_value' => $middle_name,
  );

  $form['input_person_details']['page_start_last_name'] = array(
    '#type' => 'textfield',
    '#title' => 'Achternaam',
    '#required' => TRUE,
    '#default_value' => $last_name,
  );

  $form['input_person_details']['page_start_email'] = array(
    '#type' => 'textfield',
    '#title' => 'E-mailadres',
  );

  $form['input_person_details']['page_start_phone'] = array(
    '#type' => 'textfield',
    '#title' => 'Telefoonnummer',
  );

  $form['input_person_details']['page_start_birth_date'] = array(
    '#title'           => 'Geboortedatum',
    '#type'            => 'date_select',
    '#date_format'     => 'd-m-Y',
    '#date_year_range' => '-14:-120',
    '#required' => TRUE,
    '#date_label_position' => 'within',
  );

  $form['input_address'] = array(
    '#type' => 'fieldset',
    '#title' => 'Adresgegevens',
  );

  include_once DRUPAL_ROOT . '/includes/locale.inc';
  $countries = country_get_list();
  $form['input_address']['page_start_country'] = array(
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
    empty($values['start']['page_start_country']) ||
    $values['start']['page_start_country'] === 'NL'
  ) {
    // Dutch address, autofill street and locality fields.
    $form['input_address']['page_start_postal_code'] = array(
      '#title'         => 'Postcode',
      '#name'          => 'page_start_postal_code',
      '#type'          => 'textfield',
      '#required' => TRUE,
      '#ajax' => array(
        'wrapper' => 'memberform_form_wrapper',
        'callback' => 'memberform_ajax_callback',
      ),
    );
    $form['input_address']['page_start_house_number'] = array(
      '#title'         => 'Huisnummer',
      '#name'          => 'page_start_house_number',
      '#type'          => 'textfield',
      '#required'      => TRUE,
      '#ajax' => array(
        'wrapper' => 'memberform_form_wrapper',
        'callback' => 'memberform_ajax_callback',
      ),
    );
    $form['input_address']['page_start_house_number_addition'] = array(
      '#title'         => 'Huisnummer toevoeging',
      '#type'          => 'textfield',
    );

    // Get address data based on postal code and house number.
    $postal_code = $storage['values']['start']['page_start_postal_code'];
    if (!empty($postal_code)) {
      $house_number = (isset($storage['values']['start']['page_start_house_number'])) ? $storage['values']['start']['page_start_house_number'] : NULL;
      $address_data = memberform_postcode_lookup($storage['values']['start']['page_start_postal_code'], $house_number);
      if (!is_object($address_data) && !empty($address_data)) {
        // Show warning when postalcode housenumber combination is not found.
        $message = '<div class="messages warning">' . $address_data . '</div>';
        $form['input_address']['page_start_house_number']['#field_suffix'] = empty($form['input_address']['page_start_house_number']['#field_suffix']) ? $message : $form['input_address']['page_start_house_number']['#field_suffix'] . $message;
      }
    }

    // Set street value.
    $form['input_address']['page_start_street'] = array(
      '#title'         => 'Straat',
      '#type'          => 'textfield',
      '#required'      => TRUE,
    );
    // Set locality value.
    $form['input_address']['page_start_locality'] = array(
      '#title'         => 'Plaats',
      '#type'          => 'textfield',
      '#required'      => TRUE,
    );
    if (!empty($storage['values']['start']['page_start_postal_code'])) {
      if (!empty($address_data->adres)) {
        $form['input_address']['page_start_street']['#default_value'] = $address_data->adres;
        $form['input_address']['page_start_street']['#value'] = $address_data->adres;
      };
      if (!empty($address_data->woonplaats)) {
        $form['input_address']['page_start_locality']['#default_value'] = $address_data->woonplaats;
        $form['input_address']['page_start_locality']['#value'] = $address_data->woonplaats;
      };
    }
  }
  else {
    // International address.
    $form['input_address']['page_start_street'] = array(
      '#title'         => 'Straat',
      '#type'          => 'textfield',
      '#required'      => TRUE,
    );
    $form['input_address']['page_start_house_number'] = array(
      '#title'         => 'Huisnummer',
      '#type'          => 'textfield',
      '#required'      => TRUE,
    );
    $form['input_address']['page_start_house_number_addition'] = array(
      '#title'         => 'Huisnummer toevoeging',
      '#type'          => 'textfield',
    );
    $form['input_address']['page_start_postal_code'] = array(
      '#title'         => 'Postcode',
      '#type'          => 'textfield',
    );
    $form['input_address']['page_start_locality'] = array(
      '#title'         => 'Plaats',
      '#type'          => 'textfield',
      '#required'      => TRUE,
    );
  }

  $form['input_membership'] = array(
    '#type' => 'fieldset',
    '#title' => 'Lidmaatschapsgegevens',
  );

  $options = array(
    'lid_sp' => 'SP',
    'lid_rood' => 'ROOD',
    'tribune' => 'Tribune',
    'spanning' => 'Spanning',
  );
  $age = memberform_get_age($values['start']['page_start_birth_date']);
  if ($age < 14 || $age > 27) {
    unset ($options['red_member']);
  }
  $form['input_membership']['page_start_membership_type'] = array(
    '#title' => 'Type lidmaatschap',
    '#type' => 'checkboxes',
    '#options' => $options,
    '#required' => TRUE,
    '#ajax' => array(
      'wrapper' => 'memberform_form_wrapper',
      'callback' => 'memberform_ajax_callback',
    ),
  );

  $memberships = $values['start']['page_start_membership_type'];
  $gift_options = array();
  $image_style = variable_get('memberform_gift_image_style', 'thumbnail');
  $translate = memberform_memberships();

  if (!empty($memberships)) {
    $form['input_gift'] = array(
      '#type' => 'fieldset',
      '#title' => 'Welkomstgeschenk',
    );
  }
  foreach ($memberships as $membership) {
    if (!empty($membership)) {
      switch ($membership) {
        case 'lid_sp':
        case 'lid_rood':
          $gift_options = array();
          $gift_map = array();
          // Get gifts data.
          $select = "SELECT * FROM {memberform_gifts} WHERE membership_type = :membership AND show_website = 1 ORDER BY position";
          $gifts = db_query($select, array(':membership' => $membership));
          foreach ($gifts as $key => $gift) {
            // Create gift mapping.
            $gift_map[$gift->id] = $gift;
            // Create gift options for select list.
            $gift_options[$gift->id] = $gift->name;
          }
          // Default selected gift.
          $selected_gift_id = empty($values['start']['page_start_select_' . $membership]) ? key($gift_options) : $values['start']['page_start_select_' . $membership];
          $selected_gift = $gift_map[$selected_gift_id];
          // Create image code.
          $image = file_load($selected_gift->image);
          $image_styled = empty($image) ? '' : theme('image_style', array('style_name' => $image_style, 'path' => $image->uri));
          $form['input_gift']['page_start_image_' . $selected_gift->id] = array(
            '#markup' => $image_styled,
          );
          // Show gift selection form.
          $form['input_gift']['page_start_select_' . $membership] = array(
            '#title' => 'Welkomstgeschenk ' . $translate[$membership],
            '#type' => (count($gift_options) > 4 ) ? 'select' : 'radios',
            '#options' => $gift_options,
            '#default_value' => $selected_gift_id,
            '#ajax' => array(
              'wrapper' => 'memberform_form_wrapper',
              'callback' => 'memberform_ajax_callback',
            ),
          );
          break;
      }
    }
  }

  if (
    $values['start']['page_start_membership_type']['lid_sp'] ||
    $values['start']['page_start_membership_type']['lid_rood']
  ) {
    $form['input_contribution'] = array(
      '#type' => 'fieldset',
      '#title' => 'Betalingsgegevens'
    );

    $form['input_contribution']['page_start_contribution_choice'] = array(
      '#title' => 'Kwartaalbijdrage lidmaatschap',
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
  }

  if ((
      $values['start']['page_start_membership_type']['lid_sp'] &&
      $values['start']['page_start_contribution_choice'] === 'custom'
    ) || (
      $values['start']['page_start_membership_type']['lid_rood'] &&
      $values['start']['page_start_contribution_choice'] === 'custom'
    )) {
    $form['input_contribution']['page_start_contribution_custom'] = array(
      '#title' => 'Kies een bedrag (minimaal 5 euro)',
      '#type' => 'textfield',
      '#required' => TRUE,
      '#field_prefix' => '<div class="inline">€</div>',
    );
  }

  if (
    $values['start']['page_start_membership_type']['lid_sp'] ||
    $values['start']['page_start_membership_type']['lid_rood']
  ) {
    $form['input_contribution']['page_start_contribution_payment_method'] = array(
      '#title' => 'Hoe wil je je lidmaatschapsbijdrage voldoen?',
      '#type' => 'radios',
      '#required' => TRUE,
      '#options' => array(
        'auto' => 'Automatische incasso',
        'accept' => 'Acceptgiro',
      ),
      '#name' => 'page_start_contribution_payment_method',
      '#ajax' => array(
        'wrapper' => 'memberform_form_wrapper',
        'callback' => 'memberform_ajax_callback',
      ),
    );
  }

  $is_member = TRUE;
  if (empty($memberships['lid_sp']) && empty($memberships['lid_rood'])) {
    $is_member = FALSE;
  }
  foreach ($memberships as $membership) {
    if (!empty($membership)) {
      switch ($membership) {
        case 'tribune':
          $options = array(
            'auto_5' => 'Automatische incasso, € 5 per kwartaal',
            'accept_24' => 'Acceptgiro, € 24 per jaar',
          );
          $form['input_contribution']['page_start_tribune_payment_method'] = array(
            '#title' => 'Betalingsgegevens Tribune',
            '#type' => 'radios',
            '#required' => TRUE,
            '#options' => $options,
            '#name' => 'page_start_tribune_payment_method',
            '#ajax' => array(
              'wrapper' => 'memberform_form_wrapper',
              'callback' => 'memberform_ajax_callback',
            ),
          );
          break;
        case 'spanning':
          if ($is_member) {
            $options = array(
              'auto_12' => 'Automatische incasso, € 12 per jaar (niet leden betalen € 20)',
              'accept_12' => 'Acceptgiro, € 12 per jaar (niet leden betalen € 24)',
            );
          }
          else {
            $options = array(
              'auto_20' => 'Automatische incasso € 20 (€ 12 voor leden)',
              'accept_24' => 'Acceptgiro € 24 (€ 12 voor leden)',
            );
          }
          $form['input_contribution']['page_start_spanning_payment_method'] = array(
            '#title' => 'Betalingsgegevens Spanning',
            '#type' => 'radios',
            '#required' => TRUE,
            '#options' => $options,
            '#name' => 'page_start_spanning_payment_method',
            '#ajax' => array(
              'wrapper' => 'memberform_form_wrapper',
              'callback' => 'memberform_ajax_callback',
            ),
          );
          break;
            }
    }
  }

  if (
    (
      (
        $values['start']['page_start_membership_type']['lid_sp'] ||
        $values['start']['page_start_membership_type']['lid_rood']
      ) && (
        $values['start']['page_start_contribution_payment_method'] === 'auto'
      )
    ) || (
      $values['start']['page_start_membership_type']['tribune'] &&
      $values['start']['page_start_tribune_payment_method'] === 'auto_5'
    ) || (
      (
        $values['start']['page_start_membership_type']['spanning']
      ) && (
        $values['start']['page_start_spanning_payment_method'] === 'auto_12' ||
        $values['start']['page_start_spanning_payment_method'] === 'auto_20'
      )
    )
  ) {
    $form['input_contribution']['page_start_iban'] = array(
      '#title' => 'Wat is je bankrekeningnummer?',
      '#type' => 'textfield',
    );
  }
}

/**
 * Validation for member form pages.
 */
function memberform_intern_page_form_validate($form, &$form_state) {
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
        if (isset($values['page_start_birth_date'])) {
          $age = memberform_get_age($values['page_start_birth_date']);
          if ($age < 14) {
            form_set_error('page_start_birth_date', 'Je bent jonger dan 14 jaar, en kunt daarom nog geen lid worden van de SP of ROOD.');
          }
        }
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
        if ($values['page_start_country'] !== 'NL') {
          unset($checks['postal_code']);
        }
        foreach ($checks as $key => $check) {
          if (!empty($values['page_start_' . $key])) {
            if (!preg_match($check['regex'], $values['page_start_' . $key])) {
              form_set_error('page_start_' . $key, $check['message']);
            }
          }
        }
        if ($values['page_start_contribution_choice'] === 'custom') {
          $amount = $values['page_start_contribution_custom'];
          $amount = memberform_clean_amount($amount);
          if ($amount === FALSE) {
            form_set_error('page_start_contribution_custom', 'Vul een getal in.');
          }
          else {
            if ($amount > 999999) {
              form_set_error('page_contribution_custom', 'Vul een kleiner bedrag in.');
            }
            $amount = str_replace('.', ',', $amount);
            if ($amount < 5) {
              form_set_error('page_start_contribution_custom', 'Kies een bedrag van minimaal 5 euro.');
            }
            $values['page_start_contribution_custom'] = $amount;
          }
        }
        if (
          $values['page_start_contribution_payment_method'] === 'auto' ||
          strpos($values['page_start_tribune_payment_method'], 'auto') === 0 ||
          strpos($values['page_start_spanning_payment_method'], 'auto') === 0
        ) {
          if ($values['page_start_country'] === 'NL') {
            $iban = strtoupper($values['page_start_iban']);
            $iban = preg_replace('/[^0-9A-Z]/', '', $iban);
            if (!memberform_check_iban($iban)) {
              form_set_error('page_start_iban', 'Vul een geldig iban nummer in.');
            }
            else {
              $values['page_start_iban'] = $iban;
            }
          }
        }
        break;
    }
  }
}

/**
 * Submit function for memberform pages.
 *
 * Set $storage['next_page'] to value of next page. The text of the submit
 * button can be set to 'submit' instead of 'next' by setting
 * $storage['finish_page'] to TRUE.
 */
function memberform_intern_page_form_submit($form, &$form_state) {
  if (isset($form_state['form_errors'])) {
    return;
  }

  $storage = &$form_state['storage'];
  $values = $storage['values']['start'];

  $created = empty($storage['created']) ? time() : $storage['created'];
  $middle_name = empty($values['page_start_middle_name']) ? '' : ' ' . $values['page_start_middle_name'];
  $name = $values['page_start_first_name'] . $middle_name . ' ' . $values['page_start_last_name'];

  // Set mobile, or phone.
  $phone = _hv($values['page_start_phone']);
  if (preg_match('/^(((\+31|0|0031)6){1}[1-9]{1}[0-9]{7})$/', $phone)) {
    $mobile = $phone;
    $phone = NULL;
  }
  else {
    $mobile = NULL;
  }

  // Set intern flag.
  if (empty($form_state['storage']['flags'])) {
    $form_state['storage']['flags'] = 'INTERN';
  }
  else {
    if (strpos($form_state['storage']['flags'], 'INTERN') === FALSE) {
      $form_state['storage']['flags'] .= ' INTERN';
    }
  }

  $fields = array(
    'initials' => NULL,
    'firstname' => _hv($values['page_start_first_name']),
    'middlename' => _hv($values['page_start_middle_name']),
    'lastname' => _hv($values['page_start_last_name']),
    'gender' => '',
    'birthdate' => _hv($values['page_start_birth_date']),
    'email' => _hv($values['page_start_email']),
    'phone' => $phone,
    'phone_mobile' => $mobile,
    'country' => _hv($values['page_start_country']),
    'zipcode' => _hv($values['page_start_postal_code']),
    'housenumber' => _hv($values['page_start_house_number']),
    'housenumber_suffix' => _hv($values['page_start_house_number_addition']),
    'street' => _hv($values['page_start_street']),
    'city' => _hv($values['page_start_locality']),
    'contribution' => '',
    'bank_account' => _hv($values['page_start_iban']),
    'payment_method' => '',
    'welcome_present_sp' => '',
    'welcome_present_rood' => '',
    'source' => NULL,
    'created' => $created,
    'verified' => $created,
    'reminded' => NULL,
    'submitted' => NULL,
    'errormsg' => NULL,
    'flags' => _hv($storage['flags']),
    'contact_id' => NULL,
    'note' => NULL,
    'form' => $storage['form_name'],
    'form_page' => $storage['current_page'],
  );

  // Set values dependent on membership type.
  $membership_type = $values['page_start_membership_type'];
  // Lid SP or ROOD.
  if ($membership_type['lid_sp'] || $membership_type['lid_rood']) {
    // Set contribution.
    if ($values['page_start_contribution_choice'] === 'custom') {
      $contribution_amount = str_replace(',', '.', $values['page_start_contribution_custom']);
    }
    else {
      $contribution_amount = str_replace(',', '.', $values['page_start_contribution_choice']);
    }
    $fields['contribution'] = _hv($contribution_amount);
    $fields['payment_method'] = _hv($values['page_start_contribution_payment_method']);
    // Lid SP and ROOD.
    if ($membership_type['lid_sp'] && $membership_type['lid_rood']) {
      $hash = memberform_get_hash('lid_sprood', $name);
      $fields['membership_type'] = 'lid_sprood';
      $fields['welcome_present_sp'] = _hv($values['page_start_select_lid_sp']);
      $fields['welcome_present_rood'] = _hv($values['page_start_select_lid_rood']);
    }
    // Lid SP.
    elseif ($membership_type['lid_sp']) {
      $hash = memberform_get_hash('lid_sp', $name);
      $fields['membership_type'] = 'lid_sp';
      $fields['welcome_present_sp'] = _hv($values['page_start_select_lid_sp']);
    }
    // Lid ROOD.
    elseif ($membership_type['lid_rood']) {
      $hash = memberform_get_hash('lid_rood', $name);
      $fields['membership_type'] = 'lid_rood';
      $fields['welcome_present_rood'] = _hv($values['page_start_select_lid_rood']);
    }
    memberform_intern_store_db($form_state, $hash, $fields);
  }
  // Lid Tribune of Spanning.
  if ($membership_type['tribune'] || $membership_type['spanning']) {
    $fields['welcome_present_sp'] = '';
    $fields['welcome_present_rood'] = '';
    // Lid Tribune.
    if ($membership_type['tribune']) {
      $hash = memberform_get_hash('tribune', $name);
      $fields['membership_type'] = 'tribune';
      $payment_info = explode('_', $values['page_start_tribune_payment_method']);
      $fields['contribution'] = _hv(memberform_clean_amount($payment_info[1]));
      $fields['payment_method'] = _hv($payment_info[0]);
      memberform_intern_store_db($form_state, $hash, $fields);
    }
    // Lid Spanning.
    if ($membership_type['spanning']){
      $hash = memberform_get_hash('spanning', $name);
      $fields['membership_type'] = 'spanning';
      $payment_info = explode('_', $values['page_start_spanning_payment_method']);
      $fields['contribution'] = _hv(memberform_clean_amount($payment_info[1]));
      $fields['payment_method'] = _hv($payment_info[0]);
      memberform_intern_store_db($form_state, $hash, $fields);
    }
  }

  // Clear form history.
  memberform_clear_form($form_state);

  drupal_set_message('Aanmelding is opgeslagen');
}

function memberform_intern_store_db(&$form_state, $hash, $fields) {
  $id = db_merge('memberform_submissions')
    ->key(array(
      'hash' => $hash,
    ))
    ->fields($fields)
    ->execute();

  if (!$id) {
    $form_state['redirect'] = array('aanmelden/fout');
  }
}
