(function ($) {
 Drupal.behaviors.searchblox = {
    attach:function ( context , settings ) {
        //alert(settings.searchblox.base_url) ;
		jQuery('#index_posts_button').click(function() {
			index_batch_of_posts(0);
		});
		var total_docs = settings.searchblox.total_docs;
		var base_url = settings.searchblox.base_url;
		var total_posts_written  = 0 ;
		var batch_size = 3;
        var total_posts_processed = 0;
		var index_batch_of_posts = function(start) {
			set_progress();
			var offset = start || 0;
			var data = {  offset: offset, batch_size: batch_size, 
				_ajax_token: settings.searchblox._token 
		   	};

			jQuery.ajax({
					url: base_url + '/admin/config/search/searchblox/index_docs',
					data: data ,
					dataType: 'json',
					type: 'GET',
					success: function(response, textStatus) { 
						if(response.error) {
							show_error(response.error) ;
						}	
						
						var increment = response.num_written;
						if (increment) {
							total_posts_written += increment;
						}
						total_posts_processed += batch_size;
						if (response.total > 0) {
							index_batch_of_posts(offset + batch_size);

						} else {
							//total_posts_processed = total_posts;
							total_posts_processed = total_posts_written ; 
							set_progress();
						}
						
					},
					error: function(jqXHR, textStatus, errorThrown) { 
						console.log(jqXHR + '  ' + errorThrown) ;
						try {
							errorMsg = JSON.parse(jqXHR.responseText).message;
						} catch (e) {
							errorMsg = jqXHR.responseText;
							show_error("An error occured while indexing documents");
						}
					}
				}
		  	);
	    };
		
		function show_error(message) {
			
			jQuery('#sb-error-wrap').prepend('<li class="sb-eror-list"></li>');
			jQuery(".sb-eror-list").text(message) ; 
			jQuery('.searchblox-error').show();
			jQuery('#synchronizing').fadeOut();
			jQuery("#msg").remove();
		
		}

		function set_progress() {
			var total_ops = total_docs ;
			var progress = total_posts_processed;
			if(progress > total_ops) { progress = total_ops; }
			var progress_width = Math.round(progress / total_ops * 245);
			if(progress_width < 10) { progress_width = 10; }
			if(progress === 0) {
				jQuery('#progress_bar').fadeIn();
			}

			if(progress >= total_ops) {
				jQuery('#index_posts_button').html('Indexing Complete!');
				jQuery('#progress_bar').fadeOut();
				jQuery('#index_posts_button').unbind();
				jQuery('#msg').remove();
			} else {
				jQuery('#index_posts_button').html('Indexing progress... ' + Math.round(progress / total_ops * 100) + '%');
			}

			jQuery('#index_doc_div :nth-child(2)').html( progress);
			jQuery('#progress_bar').find('div.bar').show().width(progress_width);

		}
    }
        
 };
})(jQuery);