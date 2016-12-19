<?php
/**
 * Form building function.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 * @param array $collection
 *   The object results from the Islandora Solr processor created in the
 *   printResults function.
 * @param IslandoraSolrResultsXquery $solr
 *   Solr results object, used in submit handler.
 *
 * @return array
 *   An array containing the form structure.
 */
function islandora_xquery_collection_form($form, &$form_state, $collection) {
  form_load_include($form_state, 'inc', 'islandora_xquery', 'includes/db');
  // This form gets called in a roundabout fashion and needs to include itself.
  form_load_include($form_state, 'inc', 'islandora_xquery', 'includes/solr_results');

  $rows = array();
  
  // TRUE because struggling with ajax isn't worth it.
  $form['#tree'] = TRUE;
  $form['islandora_xquery_collection'] = array(
    '#type' => 'textfield',
    '#default_value' => $collection->id,
    '#access' => FALSE,
  );

$xquery_options = array();
$associations = islandora_xquery_get_associations();
foreach ($associations as $association) {
  $xquery_options[$association['machine_name']] = $association['template_name'];
}
$xquery_options['Custom'] = t('Custom');

$form['xquery_controls'] = array(
  'xquery' => array(
    '#type' => 'select',
    '#title' => t('XQuery'),
    '#required' => TRUE,
    '#description' => t('Select the XQuery to run.'),
    '#options' => $xquery_options,
    '#ajax' => array(
      'wrapper' => 'xquery_inputs',
      'callback' => 'islandora_xquery_xquery_inputs_ajax',
    ),
  ),
  'xquery_inputs' => array(
    '#type' => 'item',
    '#prefix' => '<div id="xquery_inputs">',
    '#suffix' => '</div>',
  ),
  'dsids' => array(
    '#type' => 'textfield',
    '#title' => t('DSIDs'),
    '#description' => t('A comma seperated list of DSIDs to run against.'),
    '#size' => 10,
    '#required' => TRUE,
  ),
  'submit_selected' => array(
    '#type' => 'submit',
    '#value' => t('Run'),
  ),
);
if (isset($form_state['values']['xquery_controls']['xquery'])) {
  if ($form_state['values']['xquery_controls']['xquery'] == 'Custom') {
    $form['xquery_controls']['xquery_inputs']['query'] = array(
      '#type' => 'textarea',
      '#rows' => 10,
      '#cols' => 30,
      '#required' => TRUE,
      '#title' => t('Query'),
      '#description' => t('Enter a custom XQuery to run.'),
    );
  }
  else {
    $tokens = islandora_xquery_get_tokens_from_machine_name(
        $form_state['values']['xquery_controls']['xquery']
    );
    $form['xquery_controls']['xquery_inputs']['tokens'] = array();
    foreach ($tokens as $token) {
      $form['xquery_controls']['xquery_inputs']['tokens'][$token['token_name']] = array(
        '#type' => 'textfield',
        '#default_value' => $token['token_default'],
        '#title' => check_plain($token['token_name']),
        '#description' => check_plain($token['token_description']),
      );
    }
  }
}
  return $form;
}

function islandora_xquery_collection_form_submit(array $form, array &$form_state) {
    form_load_include($form_state, 'inc', 'islandora_collection_search', 'includes/batch');
    $collection = $form_state['values']['islandora_xquery_collection'];

    $collection_members = array();
    islandora_collection_search_get_descendants($collection, $collection_members);
    $form_state['values']['islandora_xquery_table'] = array_combine($collection_members, $collection_members);
    
    form_load_include($form_state, 'inc', 'islandora_xquery', 'includes/solr_results');
    islandora_xquery_results_form_submit($form, $form_state);
}