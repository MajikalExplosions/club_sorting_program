using ClubSortingProgram2.Solver;
using Godot;

public class MainScreen : Control
{
    public static MainScreen Instance;
    private FileDialog _PopupSelector;
    private AcceptDialog _PopupError;
    private WindowDialog _PopupHelp;

    // Called when the node enters the scene tree for the first time.
    public override void _Ready()
    {
        _PopupSelector = (FileDialog)GetNode("Popup_Selector");
        _PopupSelector.PopupExclusive = true;

        _PopupError = (AcceptDialog)GetNode("Popup_Error");
        _PopupError.PopupExclusive = true;

        _PopupHelp = (WindowDialog)GetNode("Popup_Help");
        _PopupHelp.PopupExclusive = true;
    }

    // Called every frame. 'delta' is the elapsed time since the previous frame. public override
    private void _Process(float delta)
    {
    }

    public void OnHelpPressed()
    {
        _PopupHelp.PopupCentered();
    }

    public void OnFileSelectPressed()
    {
        _PopupSelector.PopupCentered();
    }

    public void OnFileSelectConfirmed()
    {
        string file = _PopupSelector.CurrentPath;
        try
        {
            Response response = new Response(file);
        }
        catch (SolverException e)
        {
            _ShowException(e);
        }
    }

    private void _ShowException(SolverException e)
    {
        _PopupError.DialogText = e.Message;
        _PopupError.PopupCentered();
    }
}