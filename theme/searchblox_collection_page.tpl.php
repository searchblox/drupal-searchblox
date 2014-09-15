<?php 
    if( $error == 1 ) :  // If settings not configured return . 
	  return ;  
	endif ;  
   
   $_SESSION['searchblox']['_token'] = drupal_get_token() ;
   $collections = array() ;   
   $collections = $collection ;

    if(isset( $_POST['submit'] ) ) : 
       
		if(  $_SESSION['searchblox']['_token'] != $_POST['_token'] ) 
		return ; 
		
		if( ! empty ( $_POST['searchblox_check'] ) ) : 
	
			// Good idea to make sure things are set before using them
			$_POST['searchblox_check'] =  (array) $_POST['searchblox_check'] ;

			// Any of the WordPress data validation functions can be used here
			$_POST['searchblox_check'] = array_map( 'check_plain', $_POST['searchblox_check'] );
	
			
            variable_set('searchblox_search_collection' ,  $_POST['searchblox_check'] ) ; 			
		else : 
	    	 variable_set('searchblox_search_collection' ,  '' ) ; 			
			
		endif ;
		
	endif;
?>

    <?php searchblox_admin_header() ; ?> 
	<h2> SearchBlox Collection </h2>
	<h4> Mark the collections you want to show your search results from.</h4> 
	<p><b>NOTE :</b>  If not marked, it will search from all of the available collections on your server. </p>
	<hr />
  <form method="post"> 
  <table class="table table-striped sticky-enabled tableheader-processed sticky-table">
	
    <thead>
        <tr>
			<th class = "searchblox_th">
				<input type="checkbox" id="sb_checkall" name="sb_checkall" 
					<?php echo $checked = isset( $_POST['sb_checkall'] ) ? 'checked="checked"' : NULL ?>
				/>   
			</th>
            <th> <span>	 ID  </span>  </th>
            <th>   Collection Name    </th>
        </tr>
    </thead>
	 <tbody>
	
<?php  
		
		foreach($collections as $collection): 
?>
        <tr>
			<th>
			<input type="checkbox" class="sb_check" name="searchblox_check[]" 
				value="<?php  echo $collection->{'@id'};  ?>" <?php searchblox_form_collection( $collection ) ; ?> />  
			</th>
            <td>  <?php echo $collection->{'@id'} ; ?>     </td>
            <td>  <?php echo $collection->{'@name'} ; ?>  </td>
        </tr>
<?php 
	  endforeach;  
 ?>
	   
	 </tbody>
   </table>	
  <div class = "searchblox-clear"> </div>
  <input type="hidden" name="_token" value="<?php echo $_SESSION['searchblox']['_token'] ; ?>" />
  <input type="submit" name="submit" value="Save Settings"  class="button-primary" />
 </form>	
</div>
<?php 
drupal_add_js(drupal_get_path('module', 'searchblox') . '/includes/js/searchblox_collection_page.js', array('scope' => 'footer'));