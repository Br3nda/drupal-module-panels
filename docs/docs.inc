<?php
// $Id: docs.inc,v 1.1.2.1 2008/05/18 23:46:43 sdboyer Exp $
/**
 * @file
 * This file contains no working PHP code; it exists to provide additional documentation
 * for doxygen as well as to document hooks and API functions in the standard Drupal manner.
 */

/**
 * @mainpage Panels 2 Documentation
 *
 * The Panels 2 API Documentation is intended as a guide to the innerworkings of Panels, and will primarily be of interest to Drupal themers and developers. \n\n
 * Panels is a complex and abstract system, which necessitates the use of a lot of specialized terms. If you are new to Panels, 
 * we strongly recommend you consult the @ref glossary "Panels Glossary" to familiarize yourself with the jargon.\n\n
 * 
 * @section dev_api_ref Developer API Reference
 *  - @subpage plugins_api "Plugins API"
 * 
 * 
 * Please note that this documentation is generated directly from the Panels project source code. Consequently, 
 * if you find errors or have improvements, you can file a patch in the Panels issue queue at 
 * http://drupal.org/project/issues/panels like any other patch. Documentation that is not specific
 * to a file, function, or class is in the .inc files in /panels/docs.
 */

/**
 * @page glossary Panels Glossary
 * 
 * @section terms_layout Layout Terms
 * Panels' basic purpose is structure content for viewing, and it makes use of its own set
 * of terms to describe this structure. There are three essential terms - @ref pane , @ref region , and @ref display .
 *  - @anchor pane <strong>Pane:</strong> Panes are individual content items, and are the smallest/highest-level unit of content
 *    that the Panels API itself configures and renders. If you've used the Panels 2 content 
 *    editor at all, then you're probably most familiar with panes via the 'Add Content' @ref modal
 *    where you pick out the content you'd like to add. Each one of the items on that form is
 *    a @ref content_type (or subtype); once you select one and fill out the form,
 *    you'll see a graphical representation of the new pane appear. \n\n
 *  - @anchor region <strong>Panel:</strong> Panels are the containers that hold @ref pane "Panes". When you select 
 *    a @ref layout "layout", what you're choosing is the number and arrangement of panels. The content editor
 *    represents panels quite well: panels are the blue (by default) boxes with their name at the top and the
 *    plus-shaped button that opens the 'Add content' @ref modal . And yes the namespace ambiguity is awful - 
 *    we're resolving it soon by changing this term to 'Region'. \n\n
 *  - @anchor display <strong>Display:</strong> The Display unit is the most abstract of the three layout terms. Again, the Panels 2 content editor
 *    is a good way to think about it: there's no visual representation of a display in the editor because the display IS what you're editing: it's the object that contains
 *    the @ref region "panels", and it's settings in the display object that govern, for example, which content types are available on the 
 *    'Add Content' form. \n\n
 *    Displays are abstract and often the most difficult for newcomers to Panels to understand. They're also probably the most important thing that developers
 *    implementing the Panels API in a @ref client_module "client module" need to understand, as client modules work almost 
 *    exclusively with display objects. Displays are essentially the 'root' object of the API - this is functionally true in
 *    Panels 2, but it will be literally the case after Panels is rewritten in OO for Panels 3. \n\n
 *    In addition to @ref region "Panels", displays hold almost all of the @ref terms_plugins "Panels plugins" and are required
 *    in most of the @ref mainapi "main API functions"; this is why client modules interact with the Panels API primarily through
 *    display objects.\n\n
 * @section terms_plugins Panels Plugins
 * The Panels API is essentially just a structure for negotiating its plugins. For a thorough introduction to plugins, 
 * see the introduction to the @ref plugins_api "plugins API".
 *  - @anchor arguments <strong>Arguments:</strong>
 *  - @anchor content_types <strong>Content Types:</strong>
 *  - @anchor context <strong>Context:</strong>
 *  - @anchor layouts <strong>Layouts:</strong>
 *  - @anchor styles <strong>Styles:</strong>
 *  - @anchor relationships <strong>Relationships:</strong>
 */

/**
 * @page plugins_api Panels Plugins API Reference
 * @section plugmain_intro Introduction
 * The Panels API uses a system of plugins to manage pretty much everything it does. Without the
 * plugins, the API is an empty shell; plugins define the 'what, where, and how' of Panels' behavior. Panels uses six different types of plugins. Specific reference information for each plugin type is available on the corresponding page:
 *  - @subpage plugin_arguments "Arguments"
 *  - @subpage plugin_content_types "Content Types" 
 *  - @subpage plugin_context "Context"
 *  - @subpage plugin_layouts "Layouts"
 *  - @subpage plugin_styles "Styles"
 *  - @subpage plugin_relationships "Relationships". \n\n
 * 
 * These correspond to the similarly-named subdirectories filled with .inc files - each of those .inc files declares (at least) one plugin.
 * 
 * @section plugmain_structure Structure of a Plugin
 * All panels plugins have the same basic structure. Each plugin begins with a declaration function, which the Panels API identifies either by
 * function name alone or through a combination of function name and .inc file location depending on the @ref plugmain_method "method being used".
 * Regardless of the method or plugin type, the API expects the same thing to be returned from the definition function: an array containing function names
 * and/or configuration values. The array keys to be used depend entirely on the content type, so see one of the plugin subpages for examples. \n\n 
 * 
 * Once the Panels API is aware of a plugin, it will call functions/grab configuration values from the plugin definition on an as-needed basis. (Various
 * systems minimize the number of times that a given include file needs to be loaded).
 */


/**
 * @page plugin_arguments Panels Plugins Reference: Arguments
 * 
 */

/**
 * @page plugin_content_types Panels Plugins Reference: Content Types
 * 
 */

/**
 * @page plugin_context Panels Plugins Reference: Context
 * 
 */

/**
 * @page plugin_layouts Panels Plugins Reference: Layouts
 * 
 */

/**
 * @page plugin_styles Panels Plugins Reference: Styles
 * 
 */

/**
 * @page plugin_relationships Panels Plugins Reference: Relationships
 * 
 */