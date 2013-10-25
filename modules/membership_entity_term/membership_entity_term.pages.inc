<?php

/**
 * @file
 * Page callbacks for the membership_entity_term module.
 */

/**
 * Add term options to the membership settings form.
 */
function _membership_entity_term_settings_form(&$form, &$form_state) {
  $data = $form['#data'];

  // Provide some defaults.
  $data += array(
    'available_term_lengths' => array(),
  );

  // Add membership term lengths.
  if (!isset($form_state['available_term_lengths'])) {
    $form_state['available_term_lengths'] = $data['available_term_lengths'];
  }

  $form['term_lengths'] = array(
    '#type' => 'fieldset',
    '#title' => t('Membership Term Lengths'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#tree' => TRUE,
    '#description' => t('Select the term lengths that will be available for this membership type.'),
  );

  $wrapper_id = 'available-term-lengths-wrapper';
  $form['term_lengths']['available_term_lengths'] = array(
    '#theme' => 'membership_entity_term_available_term_lengths_element',
    '#id' => 'membership-entity-term-lengths',
    '#prefix' => '<div id="'. $wrapper_id . '">',
    '#suffix' => '</div>',
    '#element_validate' => array('membership_entity_term_length_element_validate')
  );

  foreach ($form_state['available_term_lengths'] as $delta => $term_length) {
    $form['term_lengths']['available_term_lengths'][$delta] = array(
      'term_length' => array(
        '#type' => 'value',
        '#value' => $term_length,
      ),
      'example' => array(
        '#markup' => ($term_length == 'lifetime') ? t('N/A') : format_date(strtotime($term_length), 'custom', 'Y-m-d H:i:s O'),
      ),
      'weight' => array(
        '#type' => 'weight',
        '#title' => t('Weight for term length @term_length', array('@term_length' => $term_length)),
        '#title_display' => 'invisible',
        '#default_value' => $delta,
      ),
      'remove' => array(
        '#type' => 'submit',
        '#value' => t('Remove'),
        '#name' => 'remove_' . $delta,
        '#delta' => $delta,
        '#submit' => array('membership_entity_term_length_remove'),
        '#limit_validation_errors' => array(),
        '#ajax' => array(
          'callback' => '_membership_entity_term_ajax_callback',
          'wrapper' => $wrapper_id,
          'effect' => 'fade',
        ),
      ),
    );
  }

  $form['term_lengths']['term_length_options'] = array(
    '#type' => 'select',
    '#title' => t('Term length'),
    '#required' => TRUE,
    '#options' => array(
      '1 day' => t('1 day'),
      '1 week' => t('1 week'),
      '1 month' => t('1 month'),
      '3 months' => t('3 months'),
      '6 months' => t('6 months'),
      '1 year' => t('1 year'),
      '2 years' => t('2 years'),
      '3 years' => t('3 years'),
      '4 years' => t('4 years'),
      '5 years' => t('5 years'),
      '10 years' => t('10 years'),
      'lifetime' => t('Lifetime'),
      'other' => t('Other'),
    ),
  );

  $form['term_lengths']['custom_term_length'] = array(
    '#type' => 'textfield',
    '#title' => t('Other term length'),
    '#size' => 32,
    '#description' => t('Enter a term length including number and unit (eg. 15 years).'),
    '#states' => array(
      'visible' => array(
       ':input[name="term_lengths[term_length_options]"]' => array('value' => 'other'),
      ),
      'required' => array(
        ':input[name="term_lengths[term_length_options]"]' => array('value' => 'other'),
      ),
    ),
  );

  $form['term_lengths']['add'] = array(
    '#type' => 'submit',
    '#value' => t('Add term length'),
    '#validate' => array('membership_entity_term_length_add_validate'),
    '#submit' => array('membership_entity_term_length_add'),
    '#limit_validation_errors' => array(
      array('term_lengths', 'term_length_options'),
      array('term_lengths', 'custom_term_length'),
    ),
    '#ajax' => array(
      'callback' => '_membership_entity_term_ajax_callback',
      'wrapper' => $wrapper_id,
      'effect' => 'fade',
    ),
  );

  $form['#validate'][] = 'membership_entity_term_settings_form_validate';
  $form['actions']['submit']['#limit_validation_errors'][] = array('available_term_lengths');
  $form['actions']['submit']['#limit_validation_errors'][] = array('term_lengths', 'available_term_lengths');
}

/**
 * Validate handler to add a term length.
 */
function membership_entity_term_length_add_validate($form, &$form_state) {
  $values = &$form_state['values']['term_lengths'];
  $term_length = $values['term_length_options'];

  // Validate a custom term length.
  if ($term_length == 'other') {
    $term_length = trim($values['custom_term_length']);
    $regex = '/^((\d+ (sec|second|min|minute|hour|day|week|month|year)s?)\s?)+$/i';
    if (!preg_match($regex, $term_length)) {
      form_set_error('term_lengths][custom_term_length', t('%term_length is not a valid term length. Please enter term lengths as a number and unit (eg. 15 years)', array(
        '%term_length' => $term_length,
      )));
    }
  }

  // Check for duplicates.
  if (in_array($term_length, $form_state['available_term_lengths'])) {
    form_set_error('term_lengths][term_length_options', t('The term length %term_length has already been added.', array(
      '%term_length' => $term_length,
    )));
  }
}

/**
 * Submit handler to add a term length.
 */
function membership_entity_term_length_add($form, &$form_state) {
  $values = $form_state['values']['term_lengths'];
  $term = ($values['term_length_options'] != 'other') ? $values['term_length_options'] : $term = $values['custom_term_length'];
  $form_state['available_term_lengths'][] = check_plain($term);
  $form_state['rebuild'] = TRUE;
}

/**
 * Submit handler to remove a term length.
 */
function membership_entity_term_length_remove($form, &$form_state) {
  $button = $form_state['triggering_element'];
  if (isset($button['#delta'])) {
    unset($form_state['available_term_lengths'][$button['#delta']]);
  }
  $form_state['rebuild'] = TRUE;
}

/**
 * Form validation handler for membership_entity_settings_form().
 */
function membership_entity_term_settings_form_validate($form, &$form_state) {
  // Flatten available term lengths.
  $form_state['values']['available_term_lengths'] = $form_state['values']['term_lengths']['available_term_lengths'];
  unset($form_state['values']['term_lengths']);
}

/**
 * Element validate callback for the available term lengths field.
 */
function membership_entity_term_length_element_validate($element, &$form_state, $form) {
  $items = element_children($element);

  // Make sure at least one term length is selected.
  if (empty($items)) {
    form_error($element, t('You must add at least one term length.'));
    return;
  }

  // Prepare available term_lengths.
  $term_lengths = array();
  foreach ($items as $delta) {
    $term_lengths[$element[$delta]['weight']['#value']] = $element[$delta]['term_length']['#value'];
  }
  ksort($term_lengths);
  form_set_value($element, $term_lengths, $form_state);
}

/**
 * Add term length options to the membership_entity_form.
 */
function _membership_entity_term_create_form(&$form, &$form_state) {
  $membership = $form['#membership_entity'];
  $bundle_settings = $form['#bundle_settings'];

  if (!empty($membership->is_new)) {
    $form['membership']['term_length'] = array(
      '#type' => 'radios',
      '#title' => t('Term length'),
      '#description' => t('Select the desired membership term length.'),
      '#required' => TRUE,
      '#options' => drupal_map_assoc($bundle_settings['available_term_lengths']),
    );

    $form['actions']['submit']['#submit'][] = 'membership_entity_term_create_form_submit';
  }
}

/**
 * Submit handler for creating new terms.
 */
function membership_entity_term_create_form_submit($form, &$form_state) {
  $membership = $form_state['#membership_entity'];
  $term_length = $form_state['values']['term_length'];
  $start = new DateObject('now', 'UTC');
  $term = membership_entity_term_create(array(
    'mid' => $membership->mid,
    'term' => $term_length,
    'start' => date_format_date($start, 'custom', DATE_FORMAT_DATETIME),
  ));
  membership_entity_term_save($term);
}

/**
 * Form builder to edit membership terms.
 */
function membership_entity_term_edit($form, &$form_state, $term) {
  $form['#membership_entity_term'] = $term;
  $form['#entity_type'] = 'membership_entity_term';
  $membership = $form['#membership_entity'] = membership_entity_load($term->mid);
  $form['#attributes']['class'][] = 'membership-entity-term-edit-form';
  $bundle_settings = $form['#bundle_settings'] = membership_entity_get_bundle_settings();

  if (empty($form['#bundle_settings'])) {
    return '';
  }

  // Basic information.
  foreach (array('id', 'mid') as $key) {
    $form[$key] = array(
      '#type' => 'value',
      '#value' => isset($term->{$key}) ? $term->{$key} : NULL,
    );
  }

  // Convert to user's timezone.
  $start = new DateObject($term->start, 'UTC');
  $start->setTimezone(new DateTimeZone(date_default_timezone()));
  $form['start'] = array(
    '#type' => 'date_select',
    '#title' => t('Term start date'),
    '#description' => t('Select the new membership term start date. The term end date will be calculated automatically based on the start date, term length, and any modifiers.'),
    '#date_format' => DATE_FORMAT_DATETIME,
    '#default_value' => date_format_date($start, 'custom', DATE_FORMAT_DATETIME),
    '#date_text_parts' => array('year'),
  );

  $form['term'] = array(
    '#type' => 'radios',
    '#title' => t('Term length'),
    '#description' => t('Select the new membership term length.'),
    '#default_value' => $term->term,
    '#required' => TRUE,
    '#options' => drupal_map_assoc($bundle_settings['available_term_lengths']),
  );

   // Add term modifiers.
  if (!isset($form_state['modifiers'])) {
    $form_state['modifiers'] = $term->modifiers;
  }

  $form['modifiers'] = array(
    '#type' => 'fieldset',
    '#title' => t('Term Modifiers'),
    '#description' => t('Modifiers are used to extend or reduce the term length by a set amout of time. eg. +1 month.'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#tree' => TRUE,
    '#states' => array(
      'invisible' => array(
       ':input[name="term"]' => array('value' => 'lifetime'),
      ),
    ),
  );

  $wrapper_id = 'term-modifiers-wrapper';
  $form['modifiers']['list'] = array(
    '#theme' => 'membership_entity_term_modifiers_element',
    '#id' => 'membership-entity-term-modifiers',
    '#prefix' => '<div id="'. $wrapper_id . '">',
    '#suffix' => '</div>',
  );

  foreach ($form_state['modifiers'] as $delta => $modifier) {
    $form['modifiers']['list'][$delta] = array(
      'value' => array(
        '#type' => 'value',
        '#value' => $modifier,
      ),
      'remove' => array(
        '#type' => 'submit',
        '#value' => t('Remove'),
        '#name' => 'remove_' . $delta,
        '#delta' => $delta,
        '#submit' => array('membership_entity_term_modifier_remove'),
        '#limit_validation_errors' => array(),
        '#ajax' => array(
          'callback' => '_membership_entity_term_ajax_callback',
          'wrapper' => $wrapper_id,
          'effect' => 'fade',
        ),
      ),
    );
  }

  $form['modifiers']['options'] = array(
    '#type' => 'select',
    '#title' => t('Select Modifier'),
    '#options' => array(
      '+1 day' => t('+1 day'),
      '+1 week' => t('+1 week'),
      '+1 month' => t('+1 month'),
      '+3 months' => t('+3 months'),
      '+6 months' => t('+6 months'),
      'other' => t('Other'),
    ),
  );

  $form['modifiers']['custom'] = array(
    '#type' => 'textfield',
    '#title' => t('Other modifier'),
    '#size' => 32,
    '#description' => t('Enter a term modifer sign (+-), number, and unit (eg. +1 month).'),
    '#states' => array(
      'visible' => array(
       ':input[name="modifiers[options]"]' => array('value' => 'other'),
      ),
      'required' => array(
        ':input[name="modifiers[options]"]' => array('value' => 'other'),
      ),
    ),
  );

  $form['modifiers']['add'] = array(
    '#type' => 'submit',
    '#value' => t('Add modifier'),
    '#validate' => array('membership_entity_term_modifier_add_validate'),
    '#submit' => array('membership_entity_term_modifier_add'),
    '#limit_validation_errors' => array(array('modifiers')),
    '#ajax' => array(
      'callback' => '_membership_entity_term_ajax_callback',
      'wrapper' => $wrapper_id,
      'effect' => 'fade',
    ),
  );

  $form['#validate'][] = 'membership_entity_term_edit_form_validate';

  // Add the buttons.
  $form['actions'] = array(
    '#type' => 'actions',
    '#weight' => 9999,
    'submit' => array(
      '#type' => 'submit',
      '#value' => t('Save'),
      '#submit' => array('membership_entity_term_edit_form_submit'),
    ),
    'delete' => array(
      '#type' => 'submit',
      '#value' => t('Delete'),
      '#limit_validation_errors' => array(),
      '#submit' => array('membership_entity_term_edit_form_delete'),
    ),
  );

  return $form;
}

/**
 * Validate handler to add a term modifier.
 */
function membership_entity_term_modifier_add_validate($form, &$form_state) {
  $values = &$form_state['values'];
  $modifier = $values['modifiers']['options'];

  // Validate a custom term length.
  if ($modifier == 'other') {
    $modifier = trim($values['modifiers']['custom']);
    $regex = '/^[\+-]((\d+ (sec|second|min|minute|hour|day|week|month|year)s?)\s?)+$/i';
    if (!preg_match($regex, $modifier)) {
      form_set_error('modifiers][custom', t('%modifier is not a valid term modifier. Please enter term lengths as a sign (+-), number, and unit (eg. +1 month)', array(
        '%modifier' => $modifier,
      )));
    }
  }
}

/**
 * Submit handler to add a term modifier.
 */
function membership_entity_term_modifier_add($form, &$form_state) {
  $values = $form_state['values'];
  $modifier = ($values['modifiers']['options'] != 'other') ? $values['modifiers']['options'] : $term = $values['modifiers']['custom'];
  $form_state['modifiers'][] = check_plain($modifier);
  $form_state['rebuild'] = TRUE;
}

/**
 * Submit handler to remove a term modifier.
 */
function membership_entity_term_modifier_remove($form, &$form_state) {
  $button = $form_state['triggering_element'];
  if (isset($button['#delta'])) {
    unset($form_state['modifiers'][$button['#delta']]);
  }
  $form_state['rebuild'] = TRUE;
}

/**
 * Validate the membership entity term edit form.
 */
function membership_entity_term_edit_form_validate($form, &$form_state) {
  $values = &$form_state['values'];

  // Prepare available term modifiers.
  if (empty($values['modifiers']['list']) || $values['term'] == 'lifetime') {
    form_set_value($form['modifiers'], array(), $form_state);
  }
  else {
    $modifiers = array_map(function($a){return $a['value'];}, $values['modifiers']['list']);
    form_set_value($form['modifiers'], $modifiers, $form_state);
  }

  // Convert start date back to UTC.
  $start = new DateObject($values['start'], $form['start']['#date_timezone']);
  $start->setTimezone(new DateTimeZone('UTC'));
  form_set_value($form['start'], date_format_date($start, 'custom', DATE_FORMAT_DATETIME), $form_state);
}

/**
 * Submit the membership entity term edit form.
 */
function membership_entity_term_edit_form_submit($form, &$form_state) {
  $term = $form['#membership_entity_term'];

  form_state_values_clean($form_state);
  entity_form_submit_build_entity('membership_entity_term', $term, $form, $form_state);
  $status = membership_entity_term_save($term);

  // Add the saved term object to $form_state for other modules.
  $form_state['#membership_entity_term'] = $term;
  switch ($status) {
    case SAVED_UPDATED:
      drupal_set_message(t('The membership term has been updated.'));
      $form_state['redirect'] = 'membership/' . $term->mid;
      break;
  }
}

/**
 * Submit handler to delete a membership term.
 */
function membership_entity_term_edit_form_delete($form, &$form_state) {
  $destination = array();
  if (isset($_GET['destination'])) {
    $destination = drupal_get_destination();
    unset($_GET['destination']);
  }
  $membership = $form['#membership_entity'];
  $form_state['redirect'] = array(
    'membership/' . $membership->mid . '/delete',
    array('query' => $destination),
  );
}

/**
 * Dispaly a confirmation page before delteing a membership.
 */
/*
function membership_entity_delete_confirm($form, &$form_state, $membership) {
  $form_state['memberships'] = array(
    $membership->mid => $membership,
  );

  $form['membership'] = array(
    '#theme_wrappers' => array('container'),
    '#attributes' => array(),
    '#markup' => t('You are about to delete membership %member_id.', array(
      '%member_id' => $membership->member_id,
    )),
  );

  $form['#submit'][] = 'membership_entity_multiple_delete_confirm_submit';
  return confirm_form($form, t('Are you sure you want to delete this membership?'), 'membership/' . $membership->mid, t('This action cannot be undone.'), t('Delete'), t('Cancel'));
}

/**
 * Display a confirmation page before deleting memberships.
 */
/*
function membership_entity_multiple_delete_confirm($form, &$form_state, $memberships) {
  $form_state['memberships'] = $memberships;

  $form['memberships'] = array(
    '#theme' => 'item_list',
    '#type' => 'ul',
    '#items' => array(),
    '#attributes' => array(),
  );
  foreach ($memberships as $mid => $value) {
    $member_id = db_query('SELECT member_id FROM {membership_entity} WHERE mid = :mid', array(':mid' => $mid))
      ->fetchField();
    $form['memberships']['#items'][] = check_plain($member_id);
  }
  $form['operation'] = array(
    '#type' => 'hidden',
    '#value' => 'delete',
  );
  $form['#submit'][] = 'membership_entity_multiple_delete_confirm_submit';

  $confirm_question = format_plural(count($memberships), 'Are you sure you want to delete this item?', 'Are you sure you want to delete these items?');
  return confirm_form($form, $confirm_question, 'admin/memberships', t('This action cannot be undone.'), t('Delete'), t('Cancel'));
}

/**
 * Submit the memberships delete confirm form.
 */
/*
function membership_entity_multiple_delete_confirm_submit($form, &$form_state) {
  $memberships = $form_state['memberships'];
  if ($form_state['values']['confirm']) {
    membership_entity_delete_multiple(array_keys($memberships));
    $count = count($memberships);
    drupal_set_message(format_plural($count, 'Deleted 1 membership.', 'Deleted @count memberships.'));
  }
  $form_state['redirect'] = 'admin/memberships';
}
}

/**
 * Menu callback: Renew an existing membership.
 */
function membership_entity_term_renew($form, &$form_state, $membership) {
  drupal_set_title(t('Renew membership @member_id', array('@member_id' => $membership->member_id)));

  $form['#membership_entity'] = $membership;
  $bundle_settings = membership_entity_get_bundle_settings();
  if (empty($bundle_settings)) {
    return '';
  }

  $form['term_length'] = array(
    '#type' => 'radios',
    '#title' => t('Renewal term length'),
    '#description' => t('Select the desired renewal term length.'),
    '#required' => TRUE,
    '#options' => drupal_map_assoc($bundle_settings['available_term_lengths']),
  );

  $form['actions'] = array(
    '#type' => 'actions',
    '#weight' => 9999,
    'submit' => array(
      '#type' => 'submit',
      '#value' => t('Renew'),
      '#submit' => array('membership_entity_term_renew_submit'),
    ),
  );

  return $form;
}

/**
 * [$membership description]
 * @var [type]
 */
function membership_entity_term_renew_submit($form, &$form_state) {
  $membership = $form['#membership_entity'];
  $bundle_settings = $form['#bundle_settings'];
}

/**
 * Ajax callback to update form elements.
 */
function _membership_entity_term_ajax_callback($form, $form_state) {
  if ($form['#membership_entity_settings_form']) {
    return $form['term_lengths']['available_term_lengths'];
  }

  if ($form['#form_id'] == 'membership_entity_term_edit') {
    return $form['modifiers']['list'];
  }
}