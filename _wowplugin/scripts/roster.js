
/* zarus
	var guildIdentity = {
		'guild': '%D0%97%D0%B0%20%D0%A0%D1%83%D1%81%D1%8C',
		'realm': '%D1%82%D0%BA%D0%B0%D1%87-%D1%81%D0%BC%D0%B5%D1%80%D1%82%D0%B8',
		'ranks': [0, 1, 2]
	};
*/

//-----------------------------------------------------------------------------------------------------
// Roster
//-----------------------------------------------------------------------------------------------------
var Roster = function () {
	//-------------------------------------------------------------------------------------------------
	this.wowClasses = [
		'paladin',
		'warrior',
		'shaman',
		'hunter',
		'druid',
		'rogue',
		'monk',
		'mage',
		'priest',
		'warlock',
		'deathknight',
		'demonhunter'
	];

	//-------------------------------------------------------------------------------------------------
	this.wowClassColors = {
		'paladin': '#F58CBA',
		'warrior': '#C79C6E',
		'shaman': '#0070DE',
		'hunter': '#ABD473',
		'druid': '#FF7D0A',
		'rogue': '#FFF569',
		'monk': '#00FF96',
		'mage': '#69CCF0',
		'priest': '#FFFFFF',
		'warlock': '#9482C9',
		'deathknight': '#C41F3B',
		'demonhunter': '#A330C9'
	};

	//-------------------------------------------------------------------------------------------------
	// https://mothereff.in/js-escapes
	this.wowClassesRussian = {
		'paladin': '\u041F\u0430\u043B\u0430\u0434\u0438\u043D\u044B',
		'warrior': '\u0412\u043E\u0438\u043D\u044B',
		'shaman': '\u0428\u0430\u043C\u0430\u043D\u044B',
		'hunter': '\u041E\u0445\u043E\u0442\u043D\u0438\u043A\u0438',
		'druid': '\u0414\u0440\u0443\u0438\u0434\u044B',
		'rogue': '\u0420\u0430\u0437\u0431\u043E\u0439\u043D\u0438\u043A\u0438',
		'monk': '\u041C\u043E\u043D\u0430\u0445\u0438',
		'mage': '\u041C\u0430\u0433\u0438',
		'priest': '\u0416\u0440\u0435\u0446\u044B',
		'warlock': '\u0427\u0435\u0440\u043D\u043E\u043A\u043D\u0438\u0436\u043D\u0438\u043A\u0438',
		'deathknight': '\u0420\u044B\u0446\u0430\u0440\u0438\x20\u0441\u043C\u0435\u0440\u0442\u0438',
		'demonhunter': '\u041E\u0445\u043E\u0442\u043D\u0438\u043A\u0438\x20\u043D\u0430\x20\u0434\u0435\u043C\u043E\u043D\u043E\u0432'
	};

	//-------------------------------------------------------------------------------------------------
	var rosterContainer = jQuery('#wowRosterContainer');

	// Header
	var rosterHeader = jQuery('<div></div>');
	rosterHeader.addClass('wowplugin-page-header not-selectable');
	rosterHeader.appendTo(rosterContainer);

	var rosterHeaderLabel = jQuery('<span>Roster</span>');
	rosterHeaderLabel.addClass('wowplugin-page-header-label not-selectable');
	rosterHeaderLabel.appendTo(rosterHeader);

	var rosterHeaderToolbar = jQuery('<div></div>');
	rosterHeaderToolbar.addClass('wowplugin-page-header-toolbar not-selectable');
	rosterHeaderToolbar.appendTo(rosterHeader);

	var toolbarButtonCheckout = jQuery('<button id="wowRosterCheckoutArmory"></button>');
	toolbarButtonCheckout.addClass('wowplugin-toolbar-button not-selectable');
	toolbarButtonCheckout.appendTo(rosterHeaderToolbar);

	var toolbarButtonIconCheckout = jQuery('<div></div>');
	toolbarButtonIconCheckout.addClass('wowplugin-toolbar-button-icon roster-toolbar-button-icon-checkout not-selectable');
	toolbarButtonIconCheckout.appendTo(toolbarButtonCheckout);

	// Content
	var rosterContent = jQuery('<div id="wowRosterContent"></div>');
	rosterContent.addClass('roster-content not-selectable');
	rosterContent.appendTo(rosterContainer);
};

Roster.prototype = {
	//-------------------------------------------------------------------------------------------------
	populateCharList: function (charListContainer, wowCharacters) {
		wowCharacters.sort(function (left, right) {
			return left.name.localeCompare(right.name);
		});

		for (var i = 0; i != wowCharacters.length; ++i) {
			var charContainer = jQuery('<a target="_blank"></a>');
			charContainer.addClass('char-item not-selectable');
			charContainer.attr('href', 'http://eu.battle.net/wow/ru/character/' + wowCharacters[i].realm + '/' + wowCharacters[i].name + '/simple');
			charContainer.appendTo(charListContainer);

			var charLabel = jQuery('<span>' + wowCharacters[i].name + '</span>');
			charLabel.addClass('char-item-label');
			charLabel.appendTo(charContainer);
		}
	},

	//-------------------------------------------------------------------------------------------------
	buildRosterContent: function (decodedResponse) {
		var container = jQuery('#wowRosterContent');
		container.empty();

		for (var i = 0; i != this.wowClasses.length; ++i) {
			var imageUrl = '/_wowplugin/images/class_icons/' + this.wowClasses[i] + '.png';

			var charListContainer = jQuery('<div></div>');
			charListContainer.addClass('char-list-container base-node-effects not-selectable');
			charListContainer.appendTo(container);

			var charListHeader = jQuery('<div></div>');
			charListHeader.addClass('char-list-header not-selectable');
			charListHeader.appendTo(charListContainer);

			var charListHeaderIcon = jQuery('<div></div>');
			charListHeaderIcon.addClass('char-list-header-icon');
			charListHeaderIcon.css('background-image', 'url("' + imageUrl + '")');
			charListHeaderIcon.appendTo(charListHeader);

			var charListHeaderLabel = jQuery('<span>' + this.wowClassesRussian[this.wowClasses[i]] + '</span>');
			charListHeaderLabel.addClass('char-list-header-label');
			charListHeaderLabel.css('color', this.wowClassColors[this.wowClasses[i]]);
			charListHeaderLabel.appendTo(charListHeader);

			this.populateCharList(charListContainer, decodedResponse[this.wowClasses[i] + 's']);
		}
	},

	//-------------------------------------------------------------------------------------------------
	queryRosterError: function (xhr, status, error) {
		console.log(error);
	},

	//-------------------------------------------------------------------------------------------------
	queryRosterSuccess: function (response, status, xhr) {
		var decodedResponse = null;
		
		try
		{
			decodedResponse = JSON.parse(response);
		}
		catch (exception)
		{
		}

		if (!decodedResponse) {
		}
		else if ('errorMessage' in decodedResponse) {
		}
		else
		{
			this.buildRosterContent(decodedResponse);
		}
	},

	//-------------------------------------------------------------------------------------------------
	reloadRosterContent: function (queryFromArmory) {
		jQuery.ajax({
			url: '/_wowplugin/query_roster.php',
			type: 'POST',
			data: JSON.stringify(guildIdentity),
			dataType: 'text',
			error: this.queryRosterError.bind(this),
			success: this.queryRosterSuccess.bind(this)
		});
	}
};

//-----------------------------------------------------------------------------------------------------
// Entry point
//-----------------------------------------------------------------------------------------------------
$(document).ready(function () {
	// Styles
	jQuery('<link/>', {
		rel: 'stylesheet',
		type: 'text/css',
		href: '/_wowplugin/styles/common.css'
	}).appendTo('head');

	jQuery('<link/>', {
		rel: 'stylesheet',
		type: 'text/css',
		href: '/_wowplugin/styles/roster.css'
	}).appendTo('head');

	// Run the building script
	var roster = new Roster();

	roster.reloadRosterContent(false);
});
