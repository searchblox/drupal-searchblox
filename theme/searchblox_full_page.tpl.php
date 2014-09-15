<?php 
searchblox_admin_header();
//Settings Table			
print theme( 'table', array( 'header' => $table_data['header'],  'rows' =>   $table_data['rows'] ) ); 

?>

 <div class="searchblox-clear"></div>
   <div id="msg">
	 <p>
		<b>Important:</b> Before your site is searchable, you need to synchronize your posts. Click the synchronize button below to begin the process.
	 </p>
	</div> 
 
	  <div class="searchblox-error" style="display:none;">
	   <p id="sb-error-wrap">
		  
	   </p>
	  </div>
 
	
	<div id="synchronizing">
		<a href="#" id="index_posts_button" class="gray-button">synchronize with SearchBlox</a>
		<div class="searchblox" id="progress_bar" style="display: none;">
			<div class="progress">
				<div class="bar" style="display: none;"></div>
			</div>
		</div>
	</div>
    <br />
	<hr/>
	<p>
		If you're having trouble with the indexing your site, or would like to reconfigure your search collection,<br/>
		you may clear your Configuration by clicking the button below. This will allow you to re-configure your SearchBlox module.
	</p>
	<form name="re_configure_form" method="post" >
	 <input type="submit" name="searchblox_re_configure" value="Clear Configuration"  class="btn btn-primary form-submit" />
	</form>
</div>
<?php 
drupal_add_js(drupal_get_path('module', 'searchblox') . '/includes/js/searchblox_index_doc.js', array('scope' => 'footer'));