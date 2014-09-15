 // Check and uncheck all 
	
	jQuery("#sb_checkall").click(function(){ 
		var check = jQuery("#sb_checkall").is(':checked') ; 
			if(check) 
				{    
					 jQuery('.sb_check').attr('checked', true);
					
				}
				
			    else {
					
					 jQuery('.sb_check').attr('checked', false);
				}
				
			}) ; 
	
	 // Check and uncheck all 