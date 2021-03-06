<?php

/**
 * @file
 * Contains the MembershipEntityType class.
 */

/**
 * Defines a membership type.
 */
class MembershipEntityType extends Entity {

  public $id;
  public $label;
  public $type;
  public $description;
  public $weight;
  public $data;

  /**
   * Default constructor for membership types.
   *
   * @param optional|array $values
   *   An array of default values.
   */
  public function __construct($values = array()) {
    parent::__construct($values, 'membership_entity_type');
  }

  /**
   * Returns the full url to the membership type.
   *
   * @param optional|string $context
   *   String containing context action.
   *
   * @return string
   *   URL to action from context, or default URL.
   */
  public function url($context = NULL) {
    $uri = $this->uri($context);
    return url($uri['path'], $uri);
  }

  /**
   * Returns the Drupal path to the membership type.
   *
   * @param optional|string $context
   *   String containing context action.
   *
   * @return string
   *   URI to action from context, or default URI.
   */
  public function path($context = NULL) {
    $uri = $this->uri($context);
    return $uri['path'];
  }

  /**
   * {@inheritdoc}
   */
  public function uri($context = NULL) {
    if (isset($this->entityInfo['uri callback']) && $this->entityInfo['uri callback'] == 'entity_class_uri') {
      switch ($context) {
        case 'join':
          return $this->joinUri();

        case 'admin':
          return $this->adminUri();

        default:
          return $this->defaultUri();
      }
    }
    return entity_uri($this->entityType, $this);
  }

  /**
   * Return a path the membership type join form.
   *
   * @return array
   *   Array containing the path string, keyed by "path".
   */
  public function joinUri() {
    $type_url_str = str_replace('_', '-', $this->type);
    return array('path' => 'user/join/' . $type_url_str);
  }

  /**
   * Return a path the membership type admin create form.
   *
   * @return array
   *   Array containing the path string, keyed by "path".
   */
  public function adminUri() {
    $type_url_str = str_replace('_', '-', $this->type);
    return array('path' => 'admin/memberships/add/' . $type_url_str);
  }

  /**
   * {@inheritdoc}
   */
  public function defaultUri() {
    return array('path' => 'admin/memberships/types/manage/' . $this->type);
  }

}
