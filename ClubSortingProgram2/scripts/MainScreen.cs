using ClubSortingProgram2.Solver;
using Godot;

public class MainScreen : Control
{
    public static MainScreen Instance;
    private FileDialog _PopupSelector;
    private AcceptDialog _PopupError;
    private WindowDialog _PopupHelp;
    private GridContainer _ColumnSetup;
    private Control _ColumnControl;

    private Response _selected;
    private Solver.SolverSettings _settings;

    // Called when the node enters the scene tree for the first time.
    public override void _Ready()
    {
        _PopupSelector = (FileDialog)GetNode("Popup_Selector");
        _PopupSelector.PopupExclusive = true;

        _PopupError = (AcceptDialog)GetNode("Popup_Error");
        _PopupError.PopupExclusive = true;

        _PopupHelp = (WindowDialog)GetNode("Popup_Help");
        _PopupHelp.PopupExclusive = true;

        _ColumnSetup = (GridContainer)GetNode("ColumnSetup");
        _ColumnSetup.Visible = false;

        _ColumnControl = (Control)GetNode("ColumnControl");
        _ColumnControl.Visible = false;
    }

    // Called every frame. 'delta' is the elapsed time since the previous frame. public override
    public override void _Process(float delta)
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
            _selected = new Response(file);
            //TODO Add settings info from configuration
            //TODO Verify settings are valid
            _settings = new Solver.SolverSettings();
            _DisplayPage(1);
            _ColumnSetup.Visible = true;
            _ColumnControl.Visible = true;
        }
        catch (SolverException e)
        {
            _ShowException(e);
        }
    }

    public void NextPage()
    {
        if (_selected == null) return;
        int page = int.Parse(((Label)GetNode("ColumnControl/Label_Page")).Text);
        if (_IsValidPage(page + 1)) _DisplayPage(page + 1);
    }

    public void PrevPage()
    {
        if (_selected == null) return;
        int page = int.Parse(((Label)GetNode("ColumnControl/Label_Page")).Text);
        if (_IsValidPage(page - 1)) _DisplayPage(page - 1);
    }

    private void _ShowException(SolverException e)
    {
        _PopupError.DialogText = e.Message;
        _PopupError.PopupCentered();
    }

    private void _DisplayPage(int page)
    {
        if (!_IsValidPage(page)) return;

        ((Label)GetNode("ColumnControl/Label_Page")).Text = page.ToString();
        ((Button)GetNode("ColumnControl/Button_PrevPage")).Visible = _IsValidPage(page - 1);
        ((Button)GetNode("ColumnControl/Button_NextPage")).Visible = _IsValidPage(page + 1);

        //TODO Create column things or edit existing ones
    }

    private bool _IsValidPage(int page)
    {
        int numPages = ((_settings.MaxRequests * _settings.Sections) - 1) / 10;
        return page > 0 && page <= numPages;
    }
}