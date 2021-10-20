#include "RumbleLeague.hpp"

using namespace std;
using namespace cv;

/// <summary>
/// 
/// Constructors more critical part it's to create the internal user defined C++ objects
/// { WindowCapture, RumbleLeagueVision } dependant objects of this class.
/// They will by dispatched via dinamic memory allocation, storing them on the heap 
/// and returning a pointer for each of them.
/// Be careful that if we lose those pointers, we will be leaking memory.
/// 
/// Constructors implementation: 
/// 1� -> The overloaded constructor are initializating the member variables through the C++ feature
/// "constructor initialization list", which it's the real way of **initializate** any object property.
/// This is because the compiler knows that at creation time, properties have to be initialized to the passed values.
/// 
/// 2� -> The default constructor, or better, the no args constructor in this case, it's initializing the member
/// variables shown below, and **assign** the window_capture and rumble_vision members to a new object of it's type.
/// In this way, objects are not properly initialized, because when the compiler reaches the body of a constructor, the object itself
/// it's already initialized and already exists in memory, so there what it's really going on it's just an assingment to new values.
/// Members that it's values are assigned on constructor body contains random garbage data before reach this point, and then are
/// overwritten and assigned to new values, so that's why constructor initialization list it's really more efficient
/// 
/// </summary>
RumbleLeague::RumbleLeague(const int language_id)
	: window_capture{ new WindowCapture },
	rumble_vision{ new RumbleLeagueVision },
	current_league_client_screen{ LeagueClientScreen::factory_screen(LeagueClientScreenIdentifier::MainScreen) }
{ 
	this->set_cpp_language();
}

RumbleLeague::RumbleLeague()
	: RumbleLeague{ 1 }  // 1 it's the ID for the default language (English)
{
	this->window_capture = new WindowCapture();
	this->rumble_vision = new RumbleLeagueVision();
	this->current_league_client_screen = LeagueClientScreen::factory_screen(LeagueClientScreenIdentifier::MainScreen);
	
	this->set_cpp_language();
}


/// The main interface method exposed to the Python API.
/// It's receives the query that the user entered, parse it again and decides what type
/// of action should be performed
/// 
/// TODO insted of void, should return something meaninful to the Python API, in order to talk with the user
/// if something goes wrong, of if the user tries to be smarter that Rumble ;)
void RumbleLeague::play(const std::string& user_input)
{
	// TODO Change the implementor's method name
	// 1�st -> Get a list with the posible client buttons that could possible be the desired user action
	auto matched_client_buttons = this->current_league_client_screen->find_client_button(user_input);

	
	/* 2 ->
		TODO Design a logical patter for when the matched keywords it's bigger than one...
		Should just take the first? Make a NLP processing? Just returning an string with a voice error message
		indicating that two petitions (or having two coincident results) can't(shouldn't) be processed at the same time?

		Maybe just the more coincident? or better above 80% coincident?
	*/

	/* 3 ->
		Perform an action against the League Client
	*/

	std::cout << "\n *************************" << endl;
	for (auto button : matched_client_buttons) {
		std::cout << "Founded a button candidate: " << button->identifier << endl;
		std::cout << "Path of the button candidate: " << button->image_path << endl;
	}

	const ClientButton* const& button = matched_client_buttons[0];
	cout << "Image path when button is retrieved: " << button->image_path << endl;
	this->league_client_action(button);


	/** TODO -> Design path

		The decision tree design will follow two branches.  
			**Direct decision** -> Like for example: 
				- I want to play an Aram, so from the Main Screen of the client the rle will auto
				make all the necessary steps to perform an action.
				- I want to play a ranked as a jungler and midlaner, so the same but it will auto-select the summoner position
			**Simple decision** -> Every screen contains identifiers that will match all the posible movements from this screen to the next,
				so the example above will be:
				1� Wanna play / play
				2� Ranked solo / duo
				3� midlaner, jungler
				4� Here will be the find match
				5� Autoaccept game (automatic)
				6� Choose me "Zed"
				7� Ban "Vex, it's `fuc****` broken
				8� Change n� of rune pages? (Complicated, implies slide)
				9� Change summoners
				10� Select skin (by name, or move right left)
	*/
}

/// <summary>
/// Moves the mouse and make a click on the location on the League of Legends Client.
/// Changes the pointer value what points to instance of the LeagueClientScreen child for the new one after matching a user input,
/// and performs some action 
/// </summary>
/// <param name="matched_keyword"></param>
void RumbleLeague::league_client_action(const ClientButton* const& client_button)
{
	cout << "Image path on league_client_action: " << client_button->image_path << endl;
	// So, the first thing that the method will do, should be get the value from an unique key

	// Declaring an iterator to retrieve the address of the desired key
	map<const char*, LeagueClientScreen*>::const_iterator it;
	
	// Using find() to search for the desired key found
	const char* button_identifier = client_button->identifier;
	it = this->available_league_client_screens.find(button_identifier);

	if (it == this->available_league_client_screens.end()) 
	{
		cout << "No action available for that input" << endl;
		// TODO Throw exception, really create a LeagueClientScreenIdentifier that represents the NO founded value
	}
	else 
	{
		cout << "\nKey: " << it->first << " Value: " << it->second->get_identifier() << endl;
		
		cout << "Current league screen: " << this->current_league_client_screen->get_identifier() << endl;
		
		// Second (inside the else block) should be modify the state of the attribute with the correct instance
		this->current_league_client_screen = it->second;

		cout << "NEW current league screen: " << this->current_league_client_screen->get_identifier() << endl;
	}

	// Sets the needle image for what we are looking for
	Mat needle_image;
	//const auto& needle_image_ref { &needle_image };
	this->set_needle_image(client_button, needle_image);

	/** 
		Finally, we can call the method that will perform the image matching and the mouse click event.
		There are two kind of events that will trigger the two available methods that represents the two type of actions

		First -> ::click_event() 
			The one who perform click events when the desired image it's present on the screen.

		Second -> ::wait_event()
			Runs until a certain event happens on the screen, like for example, the the auto-accept game.
			A wait event can be followed by a click event.


		NOTE: This will be optionally upgraded to more types of actions, like, for example, in game events, like detect
		enemies on the minimap, or track the neutral and enemy jungle camps... etc.

	*/



	bool moved_once = false;
	int key = 0;
	while (key != 27) // 'ESC' key
	{
		Mat video_source = this->window_capture->get_video_source();
		Mat* video_source_ptr = &video_source;

		//// Img finder. Matches the video source and the needle image and returns the point where the needle image is found inside the video source.
		Point m_loc = this->rumble_vision->find(video_source_ptr, needle_image);
		std::cout << "MATCH LOCATION -> " << m_loc << std::endl;

		// If match location
		// If statement for debug purposes. This one should be replaced for a method call that parses the input query from the user voice
		// and decides what event should run
		if (m_loc.x != 0 && m_loc.y != 0 && moved_once == false) // Moved once just acts as a control flag to only run this one time, or 
		// the mouse will be perma moving towards the match coordinates
		{
			RumbleMotion* rumble_motion = new RumbleMotion();
			rumble_motion->move_mouse_and_left_click(m_loc.x, m_loc.y);
			moved_once = true;
			delete rumble_motion;
		}

		//cv::imshow(window_name, *video_source_ptr);
		key = waitKey(60); // you can change wait time
	}

	// Prevents to leak memory and clean up resources
	cv::destroyAllWindows();

}


/**
* Helpers private methods
*/

void RumbleLeague::set_needle_image(const ClientButton* const& client_button, Mat& needle_image)
{
	cout << "Image path: " << client_button->image_path << endl;
	cv::Mat img_to_find = cv::imread(client_button->image_path, cv::IMREAD_COLOR);
	cvtColor(img_to_find, needle_image, COLOR_BGR2BGRA);
	// For some reason, when cv::imread loads the image as BGR
}

void RumbleLeague::set_cpp_language()
{
	// Switch statement prefered here 'cause potentially the API could be translated to more languages.
	// Obviously, the default case always should be setted to a default language (English in this case),
	// even if it's already an option. This is 'cause if it's selected first, the compiler don't have to check
	// the another alternatives.
	switch (language_id)
	{
		case 1:
			this->language = Language::English;
			break;
		case 2:
			this->language = Language::Spanish;
			break;
		// ... case N
		default:
			this->language = Language::English;
			break;
	}
}
