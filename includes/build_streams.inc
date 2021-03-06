<?php

/**
 * @file
 * functions to build and retrieve oac rdf datastreams in fedora
 */

/**
 * Builds initial suite of xml's for Shared Canvas installation
 *
 * @global url $base_url
 * @param string $pid
 *
 */
function build_manifest($pid) {
  module_load_include('inc', 'islandora_image_annotation', 'includes/utils');
  $dsid = islandora_image_annotation_get_dsid_from_model($pid);
  global $base_url;
  //module_load_include('inc', 'islandora', 'api/fedora_item');
  //$item = new Fedora_Item($pid);
  $item = islandora_object_load($pid);

  $title = $item->label;
  $file_array = array();
  $doc_url = "$base_url/islandora/object/$pid/datastream/$dsid/view/";
  //xml datastreams used by Shared Canvas must be echoed though ajax.
  $doc_path_url = "$base_url/islandora/anno/serve/$pid";
  $dimensions = get_height_and_width($pid);

  $height = $dimensions['height'];
  $width = $dimensions['width'];
  $file_array['Manifest'] = get_manifest($doc_url, $doc_path_url);
  $file_array['ImageAnnotations'] = get_image_annotations($doc_url, $doc_path_url, $height, $width, $title);
  $file_array['NormalSequence'] = get_normal_sequence($doc_url, $doc_path_url, $pid, $height, $width);
  $file_array['AnnotationList'] = get_annotationList($doc_url, $doc_path_url);
  $flat_pid = str_replace(':', '_', $pid);
  $file_path = file_directory_path() . "/$flat_pid/manifest";
  if (!is_dir($file_path)) {
    mkdir($file_path, 0777, TRUE);
  }
  foreach ($file_array as $name => $content) {
    //file_put_contents("$file_path/$name.xml", $content);
    //$item->add_or_modify_by_reference("$base_url/$file_path/$name.xml", $name, $name, 'text/plain', FALSE, 'Modified by Islandora API');
    $item["$name"]->content = $content;
    //file_delete("$file_path/$name.xml");
  }
  rmdir($file_path);
  rmdir(file_directory_path() . "/$flat_pid");
  return "completed";
}

/**
 * returns basic manifest for unpopulated document - ready for test
 *
 * @global url $base_url
 * @param string $pid
 * @return xml
 */
function get_manifest($doc_url, $doc_path_url) {
  $timestamp = date(DATE_W3C);
  module_load_include('inc', 'islandora_image_annotation', 'includes/rdf_builder');
  $rdf = new IslandoraRdfBuilder();

  $name = "$doc_url/Manifest";
  $children = array();
  $children[] = $rdf->build_child('rdf:type', array('rdf:resource' => 'http://www.openarchives.org/ore/terms/Aggregation'));
  $children[] = $rdf->build_child('rdf:type', array('rdf:resource' => 'http://dms.stanford.edu/ns/Manifest'));
  $children[] = $rdf->build_child('ore:aggregates', array('rdf:resource' => "$doc_url/NormalSequence"));
  $children[] = $rdf->build_child('ore:aggregates', array('rdf:resource' => "$doc_url/AnnotationList"));
  $children[] = $rdf->build_child('ore:aggregates', array('rdf:resource' => "$doc_url/ImageAnnotations"));
  $rdf->addDescriptionNode($name, $children);

  $name = "$doc_path_url/Manifest/manifest.xml";
  $children = array();
  $children[] = $rdf->build_child('ore:describes', array('rdf:resource' => "$doc_url/Manifest"));
  $children[] = $rdf->build_child('dcterms:modified', NULL, "$timestamp");
  $children[] = $rdf->build_child('dc:format', NULL, 'application/rdf+xml');
  $children[] = $rdf->build_child('rdf:type', array('rdf:resource' => 'http://www.openarchives.org/ore/terms/ResourceMap'));
  // $children[] = $rdf->build_child('dcterms:creator', array('Emic Shared Canvas'));
  $rdf->addDescriptionNode($name, $children);

  $name = "urn:uuid:07fbb971-6acb-48ce-9ed4-1789dfe5b3a0";
  $children = array();
  $children[] = $rdf->build_child('rdf:type', array('rdf:resource' => 'http://purl.org/dc/terms/Agent'));
  $children[] = $rdf->build_child('foaf:name', NULL, "SharedCanvas: Python/rdflib");
  $children[] = $rdf->build_child('foaf:mbox', NULL, "info@islandora.ca");
  $children[] = $rdf->build_child('dc:format', NULL, 'application/rdf+xml');
  $rdf->addDescriptionNode($name, $children);

  $name = "$doc_url/NormalSequence";
  $children = array();
  $children[] = $rdf->build_child('ore:isDescribedBy', array('rdf:resource' => "$doc_path_url/NormalSequence/NormalSequence.xml"));
  $children[] = $rdf->build_child('rdf:type', array('rdf:resource' => 'http://www.openarchives.org/ore/terms/Aggregation'));
  $children[] = $rdf->build_child('rdf:type', array('rdf:resource' => 'http://dms.stanford.edu/ns/Sequence'));
  $children[] = $rdf->build_child('rdf:type', array('rdf:resource' => 'http://www.w3.org/1999/02/22-rdf-syntax-ns#List'));
  $rdf->addDescriptionNode($name, $children);

  $name = "$doc_path_url/NormalSequence/NormalSequence.xml";
  $children = array();
  $children[] = $rdf->build_child('rdf:type', array('rdf:resource' => 'http://www.openarchives.org/ore/terms/ResourceMap'));
  $children[] = $rdf->build_child('dc:format', NULL, 'application/rdf+xml');
  $rdf->addDescriptionNode($name, $children);

  $name = "$doc_url/ImageAnnotations";
  $children = array();
  $children[] = $rdf->build_child('ore:isDescribedBy', array('rdf:resource' => "$doc_path_url/ImageAnnotations/ImageAnnotations.xml"));
  $children[] = $rdf->build_child('rdf:type', array('rdf:resource' => 'http://www.openarchives.org/ore/terms/Aggregation'));
  $children[] = $rdf->build_child('rdf:type', array('rdf:resource' => 'http://dms.stanford.edu/ns/ImageAnnotationList'));
  $children[] = $rdf->build_child('rdf:type', array('rdf:resource' => 'http://www.w3.org/1999/02/22-rdf-syntax-ns#List'));
  $rdf->addDescriptionNode($name, $children);

  $name = "$doc_path_url/ImageAnnotations/ImageAnnotations.xml";
  $children = array();
  $children[] = $rdf->build_child('rdf:type', array('rdf:resource' => 'http://www.openarchives.org/ore/terms/ResourceMap'));
  $children[] = $rdf->build_child('dc:format', NULL, 'application/rdf+xml');
  $rdf->addDescriptionNode($name, $children);
  return $rdf->printRDF();
}

/**
 * Returns a NormalSequence document. 
 * We consider annotate single pages, so the document refrences a single canvas.
 * @param URL $doc_url
 * @return xml
 */
function get_normal_sequence($doc_url, $doc_path_url, $pid, $height, $width) {
  module_load_include('inc', 'islandora_image_annotation', 'includes/rdf_builder');
  $timestamp = date(DATE_W3C);
  $rdf = new IslandoraRdfBuilder();
  $rdf->root->setAttribute('xmlns:exif', "http://www.w3.org/2003/12/exif/ns#");

  $name = "$doc_url/NormalSequence";
  $children = array();
  $children[] = $rdf->build_child('rdf:first', array('rdf:resource' => "$doc_url/Canvas"));
  $children[] = $rdf->build_child('rdf:rest', array('rdf:resource' => "http://www.w3.org/1999/02/22-rdf-syntax-ns#nil"));
  $children[] = $rdf->build_child('ore:aggregates', array('rdf:resource' => "$doc_url/Canvas"));
  $children[] = $rdf->build_child('rdf:type', array('rdf:resource' => "http://www.w3.org/1999/02/22-rdf-syntax-ns#List"));
  $children[] = $rdf->build_child('rdf:type', array('rdf:resource' => "http://www.openarchives.org/ore/terms/Aggregation"));
  $children[] = $rdf->build_child('rdf:type', array('rdf:resource' => "http://dms.stanford.edu/ns/Sequence"));
  $rdf->addDescriptionNode($name, $children);

  $name = "$doc_path_url/NormalSequence/NormalSequence.xml";
  $children = array();
  $children[] = $rdf->build_child('ore:describes', array('rdf:resource' => "$doc_url/NormalSequence"));
  $children[] = $rdf->build_child('dcterms:modified', NULL, "$timestamp");
  $children[] = $rdf->build_child('dc:format', NULL, 'application/rdf+xml');
  $children[] = $rdf->build_child('rdf:type', array('rdf:resource' => 'http://www.openarchives.org/ore/terms/ResourceMap'));
  // $children[] = $rdf->build_child('dcterms:creator', array('rdf:resource' => 'Emic Shared Canvas'));
  $rdf->addDescriptionNode($name, $children);

  $name = "$doc_url/Canvas";
  $children = array();
  $children[] = $rdf->build_child('exif:height', array('rdf:datatype' => "http://www.w3.org/2001/XMLSchema#integer"), $height);
  $children[] = $rdf->build_child('exif:width', array('rdf:datatype' => "http://www.w3.org/2001/XMLSchema#integer"), $width);
  $children[] = $rdf->build_child('dc:title', NULL, "$pid canvas");
  $children[] = $rdf->build_child('rdf:type', array('rdf:resource' => 'http://dms.stanford.edu/ns/Canvas'));
  $rdf->addDescriptionNode($name, $children);

  $name = "urn:uuid:07fbb971-6acb-48ce-9ed4-1789dfe5b3a0";
  $children = array();
  $children[] = $rdf->build_child('rdf:type', array('rdf:resource' => 'http://purl.org/dc/terms/Agent'));
  $children[] = $rdf->build_child('foaf:name', NULL, "SharedCanvas: Python/rdflib");
  $children[] = $rdf->build_child('foaf:mbox', NULL, "info@islandora.ca");
  $children[] = $rdf->build_child('dc:format', NULL, 'application/rdf+xml');

  return $rdf->printRDF();
}

/**
 * Creates and returns Imageannotation Stream for new Shared Canvas
 * @param url $doc_url
 * @param url $doc_path_url
 * @param integer $height
 * @param integer $width
 * @param string $title
 * @return string
 *
 */
function get_image_annotations($doc_url, $doc_path_url, $height, $width, $title) {
  module_load_include('inc', 'islandora_image_annotation', 'includes/rdf_builder');
  $timestamp = date(DATE_W3C);
  $rdf = new IslandoraRdfBuilder();
  $rdf->root->setAttribute('xmlns:exif', "http://www.w3.org/2003/12/exif/ns#");
  $rdf->root->setAttribute('xmlns:cnt', "http://www.w3.org/2008/content#");
  $rdf->root->setAttribute('xmlns:oa', "http://www.w3.org/ns/openannotation/core/");
  $option_uuid = generate_uuid();
  $first_resource = generate_uuid();
  $timestamp = date(DATE_W3C);
  $option_uuid = generate_uuid();
  $first_resource = generate_uuid();
  $timestamp = date(DATE_W3C);

  $name = "$doc_url/ImageAnnotations";
  $children = array();
  $children[] = $rdf->build_child('ore:aggregates', array('rdf:resource' => 'urn:uuid:46e74f54-a8e2-41f1-a3a1-24660472c584'));
  $children[] = $rdf->build_child('rdf:first', array('rdf:resource' => 'urn:uuid:b6cf1868-68f9-4994-90dd-240a4a959358'));
//  $children[] = $rdf->build_child('rdf:rest', array('rdf:resource' => 'http://www.w3.org/1999/02/22-rdf-syntax-ns#nil'));
  $children[] = $rdf->build_child('rdf:rest', array('rdf:nodeID' => 'bCfJsWeh546'));
  $children[] = $rdf->build_child('rdf:type', array('rdf:resource' => 'http://www.openarchives.org/ore/terms/Aggregation'));
  $children[] = $rdf->build_child('rdf:type', array('rdf:resource' => 'http://dms.stanford.edu/ns/ImageAnnotationList'));
  $children[] = $rdf->build_child('rdf:type', array('rdf:resource' => 'http://www.w3.org/1999/02/22-rdf-syntax-ns#List'));
// todo add elements for aggregated annotations
  $rdf->addDescriptionNode($name, $children);

  $name = "$doc_path_url/ImageAnnotations/ImageAnnotations.xml";
  $children = array();
  $children[] = $rdf->build_child('dcterms:modified', NULL, "$timestamp");
  $children[] = $rdf->build_child('dc:format', NULL, 'application/rdf+xml');
  $children[] = $rdf->build_child('ore:describes', array('rdf:resource' => "$doc_url/ImageAnnotations"));
//  $children[] = $rdf->build_child('dcterms:creator', array('rdf:resource' => 'Emic Shared Canvas'));
  $children[] = $rdf->build_child('rdf:type', array('rdf:resource' => 'http://www.openarchives.org/ore/terms/ResourceMap'));
  $rdf->addDescriptionNode($name, $children);

  $name = "$first_resource";
  $children = array();
  $children[] = $rdf->build_child('rdf:type', array('rdf:resource' => 'http://dms.stanford.edu/ns/ImageAnnotation'));
  $children[] = $rdf->build_child('rdf:type', array('rdf:resource' => 'http://www.w3.org/ns/openannotation/core/Annotation'));
  $children[] = $rdf->build_child('oa:hasBody', array('rdf:resource' => $option_uuid));
  $children[] = $rdf->build_child('oa:hasTarget', array('rdf:resource' => "$doc_url/Canvas"));
  $rdf->addDescriptionNode($name, $children);

  $name = "urn:uuid:e5c12519-e756-4442-a83d-556848c32714";
  $children = array();
  $children[] = $rdf->build_child('foaf:name', NULL, "SharedCanvas: Python/rdflib");
  $children[] = $rdf->build_child('foaf:mbox', NULL, "info@islandora.ca");
  $children[] = $rdf->build_child('rdf:type', array('rdf:resource' => 'http://purl.org/dc/terms/Agent'));
  $rdf->addDescriptionNode($name, $children);

  $name = "$doc_url/JPEG";
  $children = array();
  $children[] = $rdf->build_child('exif:width', array('rdf:datatype' => 'http://www.w3.org/2001/XMLSchema#integer'), $height);
  $children[] = $rdf->build_child('exif:height', array('rdf:datatype' => "http://www.w3.org/2001/XMLSchema#integer"), $width);
  $children[] = $rdf->build_child('rdf:type', array('rdf:resource' => 'http://purl.org/dc/dcmitype/Image'));
  $children[] = $rdf->build_child('rdf:type', array('rdf:resource' => 'http://dms.stanford.edu/ns/ImageBody'));
  $rdf->addDescriptionNode($name, $children);

  $name = "$doc_url/Canvas";
  $children = array();
  $children[] = $rdf->build_child('exif:width', array('rdf:datatype' => 'http://www.w3.org/2001/XMLSchema#integer'), $height);
  $children[] = $rdf->build_child('exif:height', array('rdf:datatype' => "http://www.w3.org/2001/XMLSchema#integer"), $width);
  $children[] = $rdf->build_child('dc:title', NULL, "$title");
  $children[] = $rdf->build_child('rdf:type', array('rdf:resource' => 'http://dms.stanford.edu/ns/Canvas'));
  $rdf->addDescriptionNode($name, $children);

  $name = "$option_uuid";
  $children = array();
  $children[] = $rdf->build_child('rdf:type', array('rdf:resource' => 'http://dms.stanford.edu/ns/ImageChoice'));
  $children[] = $rdf->build_child('dms:option', array('rdf:resource' => "$doc_url/JPEG"));
  $rdf->addDescriptionNode($name, $children);

  return $rdf->printRDF();
}

/**
 * Creates and returns AnnotationList for new Shared Canvas
 * @param url $doc_url
 * @param url $doc_path_url
 * @return string
 */
function get_annotationList($doc_url, $doc_path_url) {

  module_load_include('inc', 'islandora_image_annotation', 'includes/rdf_builder');
  $timestamp = date(DATE_W3C);
  $rdf = new IslandoraRdfBuilder();
  $rdf->root->setAttribute('xmlns:oa', "http://www.w3.org/ns/openannotation/core/");

  $name = "$doc_url/AnnotationList";
  $children = array();
  $children[] = $rdf->build_child('rdf:rest', array('rdf:resource' => "http://www.w3.org/1999/02/22-rdf-syntax-ns#nil"));
  $children[] = $rdf->build_child('rdf:type', array('rdf:resource' => "http://dms.stanford.edu/ns/AnnotationList"));
  $children[] = $rdf->build_child('rdf:type', array('rdf:resource' => "http://www.w3.org/1999/02/22-rdf-syntax-ns#List"));
  $children[] = $rdf->build_child('rdf:type', array('rdf:resource' => "http://www.openarchives.org/ore/terms/Aggregation"));
  $rdf->addDescriptionNode($name, $children);

  $name = "$doc_url/Canvas";
  $children = array();
  $children[] = $rdf->build_child('rdf:type', array('rdf:resource' => "http://dms.stanford.edu/ns/Canvas"));
  $rdf->addDescriptionNode($name, $children);

  $name = "$doc_path_url/AnnotationList/AnnotationList.xml";
  $children = array();
  $children[] = $rdf->build_child('ore:describes', array('rdf:resource' => "$doc_url/AnnotationList"));
  $children[] = $rdf->build_child('rdf:type', array('rdf:resource' => ""));
  $children[] = $rdf->build_child('dc:format', NULL, 'application/rdf+xml');
  $children[] = $rdf->build_child('dcterms:modified', NULL, "$timestamp");
  $rdf->addDescriptionNode($name, $children);

  return $rdf->printRDF();
}

/**
 * Retrieves height and width from the RELS-INT  if its there
 * falls back to djatoka if it's not there.  
 * @param pid $pid
 * @return array
 * Array has two values - height and width
 */
function get_height_and_width($pid) {
  module_load_include('inc', 'islandora_image_annotation', 'includes/utils');
  $dsid = islandora_image_annotation_get_dsid_from_model($pid);
  $item = islandora_object_load($pid);
  $height_width_arr = NULL;
  if (isset($item['RELS-INT'])) {
    $content = $item['RELS-INT']->content;
    $sxml = simplexml_load_string($content);
    $sxml->registerXPathNamespace('islandora-page', "info:islandora/islandora-system:def/pageinfo#");
    $sxml->registerXPathNamespace('islandora-relsint', 'http://islandora.ca/ontology/relsint#');
    $sxml->registerXPathNamespace('rdf', 'http://www.w3.org/1999/02/22-rdf-syntax-ns#');
    $sxml->registerXPathNamespace('fits', "http://hul.harvard.edu/ois/xml/ns/fits/fits_output");
    $image_height = $sxml->xpath('//rdf:Description[@rdf:about="info:fedora/' . $pid . '/' . $dsid . '"]/islandora-relsint:height');
    $image_width = $sxml->xpath('//rdf:Description[@rdf:about="info:fedora/' . $pid . '/' . $dsid . '"]/islandora-relsint:width');
    if (!$image_height || !$image_width) {
      //we maybe looking at a page of a book which might have a different namespace
      $image_height = $sxml->xpath('//rdf:Description[@rdf:about="info:fedora/' . $pid . '/' . $dsid . '"]/islandora-page:height');
      $image_width = $sxml->xpath('//rdf:Description[@rdf:about="info:fedora/' . $pid . '/' . $dsid . '"]/islandora-page:width');
    }
    if (!$image_height || !$image_width) {
      //have seen fits:imageHeight fits:imageWidth in the wild
      $image_height = $sxml->xpath('//rdf:Description[@rdf:about="info:fedora/' . $pid . '/' . $dsid . '"]/fits:imageHeight');
      $image_width = $sxml->xpath('//rdf:Description[@rdf:about="info:fedora/' . $pid . '/' . $dsid . '"]/fits:imageWidth');
    }
    //we could have a rels-int without a height and width for our object
    if (!$image_height || !$image_width) {
      $height_width_arr = get_image_dimenions_drupal($pid, $dsid, TRUE);
    }
    else {
      $image_height = (string) $image_height[0];
      $image_width = (string) $image_width[0];
      $height_width_arr = array(
        'height' => $image_height,
        'width' => $image_width,
      );
    }
  }
  else {//fall back to djatoka  
    return get_image_dimenions_drupal($pid, $dsid, TRUE);
  }
  return $height_width_arr;
}

/**
 *
 * @param string $pid
 * @param string $dsid
 * @param boolean $update
 *  if true we will call another function to update the RELS-INT with the height and width
 * @return array
 * @throws Exception 
 *   if the target datastream does not exist 
 */
function get_image_dimenions_drupal($pid, $dsid, $update = FALSE) {

  if (empty($dsid)) {
    throw Exception('Could not find a datastream id to load for image height and width');
  }
  $object = islandora_object_load($pid);
  $image_file = file_save_data($object[$dsid]->content, 'temporary://');
  //$image_file->filemime = $object[$dsid]->mimetype;
  $real_path = drupal_realpath($image_file->uri);
  $info = image_get_info($real_path);
  if (empty($info)) {
    drupal_set_message(t('There was an error getting the image size.'), 'error');
    return NULL;
  }

  if ($update) {
    update_or_create_relsint($pid, $dsid, $info);
  }
  file_delete($image_file);
  return $info; //$height_width_arr;
}

/**
 * TODO: modify this function as it could be DESTRUCTIVE if something else is 
 * being stored in RELS-INT
 * update the object so the RELS-INT datastream contains the height and width of the 
 * selected datastream.  We are using the standard islandora RELS-INT namespace
 * @param string $pid
 * @param string $dsid
 * @param array $height_width_arr 
 */
function update_or_create_relsint($pid, $dsid, $height_width_arr) {
  if (!isset($height_width_arr['width']) || !isset($height_width_arr['height'])) {
    watchdog('islandora', 'Error adding RELS-INT stream for object %pid. no height or width specified', array('pid%' => $pid));
  }
  $rels_int_str = <<<XML
    <rdf:RDF xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#">
  <rdf:Description rdf:about="info:fedora/XPID/XTIFF">
    <width xmlns="http://islandora.ca/ontology/relsint#">XWIDTH</width>
    <height xmlns="http://islandora.ca/ontology/relsint#">XHEIGHT</height>
  </rdf:Description>
</rdf:RDF>
XML;
  $item = islandora_object_load($pid);
  if (!isset($item[$dsid])) {
    //no datastream to create a rels-int for
    return FALSE;
  }
  if (!isset($item['RELS-INT'])) {
    $to_replace = array('XPID', 'XWIDTH', 'XHEIGHT', 'XTIFF');
    $replace_with = array($pid, $height_width_arr['width'], $height_width_arr['height'], $dsid);
    $rels_int_str = str_replace($to_replace, $replace_with, $rels_int_str);
    try {
      $rels_int_ds = $item->constructDatastream('RELS-INT', 'X');
      $rels_int_ds->mimetype = 'text/xml';
      $rels_int_ds->label = 'RELS-INT';
      $rels_int_ds->content = $rels_int_str;
      $item->ingestDatastream($rels_int_ds); //create rels-int
    } catch (Exception $e) {
      watchdog('islandora', 'Error adding RELS-INT stream for object %pid', array('pid%' => $pid));
    }
  }
  else {
    //we are assuming our entries do not exist as we have just tried to load this info
    $rels_ds = $item['RELS-INT'];
    $doc = DomDocument::loadXML($rels_ds->content);
    $rdf = $doc->documentElement;
    $description = $doc->createElement('rdf:Description');
    $about = $doc->createAttribute('rdf:about');
    $about->value = "info:fedora/$pid/$dsid";
    $description->appendChild($about);
    $width = $doc->createElement('width', $height_width_arr['width']);
    $height = $doc->createElement('height', $height_width_arr['width']);
    $width->setAttribute('xmlns', "http://islandora.ca/ontology/relsint#");
    $height->setAttribute('xmlns', "http://islandora.ca/ontology/relsint#");
    $description->appendChild($width);
    $description->appendChild($height);
    $rdf->appendChild($description);
    $xml = $doc->saveXML();
    $item['RELS-INT']->content = $xml;
  }
}

/**
 * Echos Fedora streams for javascript retrieval
 * @global url $base_url
 * @param string $pid
 * @param string $stream
 */
function islandora_serve($pid, $stream, $width = NULL, $height = NULL) {
  global $base_url;
  module_load_include('inc', 'islandora_image_annotation', 'includes/utils');
  $dsid = islandora_image_annotation_get_dsid_from_model($pid);
  $doc_url = "$base_url/islandora/object/$pid/datastream/$dsid/view";
  $doc_path_url = "$base_url/islandora/anno/serve/$pid";

  if ($stream == 'Manifest') {
    $manifest = get_manifest($doc_url, $doc_path_url);
    echo $manifest;
    return;
  }
  $dimensions = get_height_and_width($pid);

  $height = $dimensions['height'];
  $width = $dimensions['width'];



  if ($stream == 'NormalSequence') {
    $sequence = get_normal_sequence($doc_url, $doc_path_url, $pid, $height, $width);
    echo $sequence;
    return;
  }
  if ($stream == 'ImageAnnotations') {
    $image_annotations = get_image_annotations($doc_url, $doc_path_url, $height, $width, NULL);
    echo $image_annotations;
    return;
  }
}

/**
 * Generates uuid
 * @return uuid
 */
function generate_uuid() {
  return sprintf('%04x%04x-%04x-%04x-%04x-%04x%04x%04x', mt_rand(0, 0xffff), mt_rand(0, 0xffff), mt_rand(0, 0xffff), mt_rand(0, 0x0fff) | 0x4000, mt_rand(0, 0x3fff) | 0x8000, mt_rand(0, 0xffff), mt_rand(0, 0xffff), mt_rand(0, 0xffff)
  );
}