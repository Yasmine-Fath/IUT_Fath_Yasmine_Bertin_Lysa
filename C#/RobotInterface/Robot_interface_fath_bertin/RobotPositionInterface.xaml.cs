using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Shapes;

namespace Robot_interface_fath_bertin
{
    public partial class RobotPositionInterface : UserControl
    {
        // ============================================================
        // DERNIERES VALEURS RECUES DE L'EMBARQUE
        // ============================================================

        private double xGhost = 0.0;
        private double yGhost = 0.0;
        private double thetaGhost = 0.0;

        private double xWaypoint = 0.0;
        private double yWaypoint = 0.0;
        private double thetaWaypoint = 0.0;

        private double distanceWP = 0.0;

        // ============================================================
        // PARAMETRES GRAPHIQUES
        // ============================================================

        private double echelle = 100.0;

        private double longueurRobot = 0.40;
        private double largeurRobot = 0.30;

        // Longueur de la flèche du waypoint
        private double longueurFlecheWaypoint = 0.30;

        // ============================================================
        // CONSTRUCTEUR
        // ============================================================

        public RobotPositionInterface()
        {
            InitializeComponent();

            SizeChanged += RobotPositionInterface_SizeChanged;
            Loaded += RobotPositionInterface_Loaded;
        }

        // ============================================================
        // INITIALISATION
        // ============================================================

        private void RobotPositionInterface_Loaded(
            object sender,
            RoutedEventArgs e)
        {
            Dessiner();
        }

        private void RobotPositionInterface_SizeChanged(
            object sender,
            SizeChangedEventArgs e)
        {
            Dessiner();
        }

        // ============================================================
        // FONCTION PRINCIPALE
        // ============================================================
        //
        // CETTE FONCTION DOIT ETRE APPELEE A CHAQUE RECEPTION
        // D'UNE NOUVELLE TRAME VENANT DE L'EMBARQUE.
        //
        // Elle conserve toujours la DERNIERE valeur reçue.
        //
        // AUCUN CALCUL DE TRAJECTOIRE N'EST EFFECTUE ICI.
        //
        // ============================================================

        public void AfficherPosition(
            double xG,
            double yG,
            double thetaG,
            double xWP,
            double yWP,
            double thetaWP,
            double distance)
        {
            // --------------------------------------------------------
            // On prend uniquement les valeurs reçues
            // --------------------------------------------------------

            this.xGhost = xG;
            this.yGhost = yG;
            this.thetaGhost = thetaG;

            this.xWaypoint = xWP;
            this.yWaypoint = yWP;
            this.thetaWaypoint = thetaWP;

            this.distanceWP = distance;

            // --------------------------------------------------------
            // AFFICHAGE IMMEDIAT
            // --------------------------------------------------------

            Dessiner();
        }

        // ============================================================
        // CONVERSION METRES -> PIXELS
        // ============================================================

        private Point CoordonneesVersCanvas(
            double x,
            double y)
        {
            double origineX =
                canvasRobot.ActualWidth / 2.0;

            double origineY =
                canvasRobot.ActualHeight / 2.0;

            double pixelX =
                origineX + x * echelle;

            double pixelY =
                origineY - y * echelle;

            return new Point(
                pixelX,
                pixelY);
        }

        // ============================================================
        // DESSIN GENERAL
        // ============================================================

        private void Dessiner()
        {
            if (canvasRobot == null)
                return;

            if (canvasRobot.ActualWidth <= 0 ||
                canvasRobot.ActualHeight <= 0)
            {
                return;
            }

            canvasRobot.Children.Clear();

            DessinerAxes();
            DessinerLiaisonGhostWaypoint();
            DessinerWaypoint();
            DessinerGhost();
        }

        // ============================================================
        // AXES
        // ============================================================

        private void DessinerAxes()
        {
            double largeur =
                canvasRobot.ActualWidth;

            double hauteur =
                canvasRobot.ActualHeight;

            double origineX =
                largeur / 2.0;

            double origineY =
                hauteur / 2.0;

            // --------------------------------------------------------
            // AXE X
            // --------------------------------------------------------

            Line axeX = new Line
            {
                X1 = 0,
                Y1 = origineY,

                X2 = largeur,
                Y2 = origineY,

                Stroke = Brushes.LightGray,
                StrokeThickness = 1
            };

            canvasRobot.Children.Add(axeX);

            // --------------------------------------------------------
            // AXE Y
            // --------------------------------------------------------

            Line axeY = new Line
            {
                X1 = origineX,
                Y1 = 0,

                X2 = origineX,
                Y2 = hauteur,

                Stroke = Brushes.LightGray,
                StrokeThickness = 1
            };

            canvasRobot.Children.Add(axeY);

            // --------------------------------------------------------
            // FLECHE X
            // --------------------------------------------------------

            Polygon flecheX = new Polygon
            {
                Fill = Brushes.Gray,

                Points = new PointCollection
                {
                    new Point(
                        largeur - 5,
                        origineY),

                    new Point(
                        largeur - 15,
                        origineY - 5),

                    new Point(
                        largeur - 15,
                        origineY + 5)
                }
            };

            canvasRobot.Children.Add(flecheX);

            // --------------------------------------------------------
            // FLECHE Y
            // --------------------------------------------------------

            Polygon flecheY = new Polygon
            {
                Fill = Brushes.Gray,

                Points = new PointCollection
                {
                    new Point(
                        origineX,
                        5),

                    new Point(
                        origineX - 5,
                        15),

                    new Point(
                        origineX + 5,
                        15)
                }
            };

            canvasRobot.Children.Add(flecheY);

            // --------------------------------------------------------
            // LABEL X
            // --------------------------------------------------------

            TextBlock texteX = new TextBlock
            {
                Text = "X",
                FontWeight = FontWeights.Bold,
                Foreground = Brushes.Gray
            };

            Canvas.SetLeft(
                texteX,
                largeur - 25);

            Canvas.SetTop(
                texteX,
                origineY + 5);

            canvasRobot.Children.Add(texteX);

            // --------------------------------------------------------
            // LABEL Y
            // --------------------------------------------------------

            TextBlock texteY = new TextBlock
            {
                Text = "Y",
                FontWeight = FontWeights.Bold,
                Foreground = Brushes.Gray
            };

            Canvas.SetLeft(
                texteY,
                origineX + 5);

            Canvas.SetTop(
                texteY,
                5);

            canvasRobot.Children.Add(texteY);
        }

        // ============================================================
        // LIAISON GHOST -> WAYPOINT
        // ============================================================

        private void DessinerLiaisonGhostWaypoint()
        {
            Point ghost =
                CoordonneesVersCanvas(
                    xGhost,
                    yGhost);

            Point waypoint =
                CoordonneesVersCanvas(
                    xWaypoint,
                    yWaypoint);

            Line ligne = new Line
            {
                X1 = ghost.X,
                Y1 = ghost.Y,

                X2 = waypoint.X,
                Y2 = waypoint.Y,

                Stroke = Brushes.OrangeRed,
                StrokeThickness = 2,

                StrokeDashArray =
                    new DoubleCollection
                    {
                        5,
                        3
                    }
            };

            canvasRobot.Children.Add(ligne);
        }

        // ============================================================
        // WAYPOINT
        // ============================================================

        private void DessinerWaypoint()
        {
            Point centre =
                CoordonneesVersCanvas(
                    xWaypoint,
                    yWaypoint);

            // --------------------------------------------------------
            // CERCLE DU WAYPOINT
            // --------------------------------------------------------

            Ellipse cercle = new Ellipse
            {
                Width = 18,
                Height = 18,

                Fill = Brushes.Transparent,
                Stroke = Brushes.Red,
                StrokeThickness = 3
            };

            Canvas.SetLeft(
                cercle,
                centre.X - cercle.Width / 2);

            Canvas.SetTop(
                cercle,
                centre.Y - cercle.Height / 2);

            canvasRobot.Children.Add(cercle);

            // --------------------------------------------------------
            // CROIX HORIZONTALE
            // --------------------------------------------------------

            Line ligneX = new Line
            {
                X1 = centre.X - 10,
                Y1 = centre.Y,

                X2 = centre.X + 10,
                Y2 = centre.Y,

                Stroke = Brushes.Red,
                StrokeThickness = 2
            };

            canvasRobot.Children.Add(ligneX);

            // --------------------------------------------------------
            // CROIX VERTICALE
            // --------------------------------------------------------

            Line ligneY = new Line
            {
                X1 = centre.X,
                Y1 = centre.Y - 10,

                X2 = centre.X,
                Y2 = centre.Y + 10,

                Stroke = Brushes.Red,
                StrokeThickness = 2
            };

            canvasRobot.Children.Add(ligneY);

            // --------------------------------------------------------
            // FLECHE DU THETA WAYPOINT
            // --------------------------------------------------------

            double longueur =
                longueurFlecheWaypoint * echelle;

            double cos =
                Math.Cos(thetaWaypoint);

            double sin =
                Math.Sin(thetaWaypoint);

            // La flèche part EXACTEMENT du centre du waypoint.
            Point pointe = new Point(
                centre.X +
                cos * longueur,

                centre.Y -
                sin * longueur
            );

            Line direction = new Line
            {
                X1 = centre.X,
                Y1 = centre.Y,

                X2 = pointe.X,
                Y2 = pointe.Y,

                Stroke = Brushes.Red,
                StrokeThickness = 3
            };

            canvasRobot.Children.Add(direction);

            // --------------------------------------------------------
            // PETITE POINTE DE FLECHE
            // --------------------------------------------------------

            double angleFleche = 25.0 * Math.PI / 180.0;

            double longueurPointe = 0.08 * echelle;

            Point pointeGauche =
                new Point(
                    pointe.X -
                    longueurPointe *
                    Math.Cos(thetaWaypoint - angleFleche),

                    pointe.Y +
                    longueurPointe *
                    Math.Sin(thetaWaypoint - angleFleche)
                );

            Point pointeDroite =
                new Point(
                    pointe.X -
                    longueurPointe *
                    Math.Cos(thetaWaypoint + angleFleche),

                    pointe.Y +
                    longueurPointe *
                    Math.Sin(thetaWaypoint + angleFleche)
                );

            Polygon fleche = new Polygon
            {
                Fill = Brushes.Red,

                Points = new PointCollection
                {
                    pointe,
                    pointeGauche,
                    pointeDroite
                }
            };

            canvasRobot.Children.Add(fleche);

            // --------------------------------------------------------
            // TEXTE WAYPOINT
            // --------------------------------------------------------

            double angleDegres =
                thetaWaypoint *
                180.0 /
                Math.PI;

            TextBlock texte = new TextBlock
            {
                Text =
                    $"WP\n" +
                    $"x = {xWaypoint:F2} m\n" +
                    $"y = {yWaypoint:F2} m\n" +
                    $"θ = {angleDegres:F1}°\n" +
                    $"Distance = {distanceWP:F2} m",

                Foreground = Brushes.Red,
                FontWeight = FontWeights.Bold
            };

            Canvas.SetLeft(
                texte,
                centre.X + 15);

            Canvas.SetTop(
                texte,
                centre.Y - 60);

            canvasRobot.Children.Add(texte);
        }

        // ============================================================
        // GHOST / ROBOT
        // ============================================================

        private void DessinerGhost()
        {
            Point centre =
                CoordonneesVersCanvas(
                    xGhost,
                    yGhost);

            double longueur =
                longueurRobot * echelle;

            double largeur =
                largeurRobot * echelle;

            // --------------------------------------------------------
            // ORIENTATION DU GHOST
            // --------------------------------------------------------

            double cos =
                Math.Cos(thetaGhost);

            double sin =
                Math.Sin(thetaGhost);

            // --------------------------------------------------------
            // POINTE DU ROBOT
            // --------------------------------------------------------

            Point pointe = new Point(
                centre.X +
                cos * longueur / 2.0,

                centre.Y -
                sin * longueur / 2.0
            );

            // --------------------------------------------------------
            // ARRIERE GAUCHE
            // --------------------------------------------------------

            Point arriereGauche = new Point(
                centre.X
                - cos * longueur / 2.0
                + sin * largeur / 2.0,

                centre.Y
                + sin * longueur / 2.0
                + cos * largeur / 2.0
            );

            // --------------------------------------------------------
            // ARRIERE DROIT
            // --------------------------------------------------------

            Point arriereDroit = new Point(
                centre.X
                - cos * longueur / 2.0
                - sin * largeur / 2.0,

                centre.Y
                + sin * longueur / 2.0
                - cos * largeur / 2.0
            );

            // --------------------------------------------------------
            // CORPS DU ROBOT
            // --------------------------------------------------------

            Polygon robot = new Polygon
            {
                Fill = Brushes.DodgerBlue,
                Stroke = Brushes.DarkBlue,
                StrokeThickness = 2,

                Points = new PointCollection
                {
                    pointe,
                    arriereGauche,
                    arriereDroit
                }
            };

            canvasRobot.Children.Add(robot);

            // --------------------------------------------------------
            // CENTRE / BARYCENTRE DU ROBOT
            // --------------------------------------------------------

            Ellipse centreRobot = new Ellipse
            {
                Width = 8,
                Height = 8,

                Fill = Brushes.White,
                Stroke = Brushes.DarkBlue,
                StrokeThickness = 2
            };

            Canvas.SetLeft(
                centreRobot,
                centre.X -
                centreRobot.Width / 2);

            Canvas.SetTop(
                centreRobot,
                centre.Y -
                centreRobot.Height / 2);

            canvasRobot.Children.Add(centreRobot);

            // --------------------------------------------------------
            // TEXTE GHOST
            // --------------------------------------------------------

            double angleDegres =
                thetaGhost *
                180.0 /
                Math.PI;

            TextBlock texte = new TextBlock
            {
                Text =
                    $"Ghost\n" +
                    $"x = {xGhost:F2} m\n" +
                    $"y = {yGhost:F2} m\n" +
                    $"θ = {angleDegres:F1}°",

                Foreground = Brushes.DarkBlue,
                FontWeight = FontWeights.Bold
            };

            Canvas.SetLeft(
                texte,
                centre.X + 15);

            Canvas.SetTop(
                texte,
                centre.Y + 10);

            canvasRobot.Children.Add(texte);
        }
    }
}









/*using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Shapes;


namespace Robot_interface_fath_bertin
{
    public partial class RobotPositionInterface : UserControl
    {
        // ============================================================
        // DONNEES
        // ============================================================

        // Position du Ghost
        private double xGhost = 0.0;
        private double yGhost = 0.0;

        // Orientation du Ghost en radians
        private double thetaGhost = 0.0;

        // Position du Waypoint
        private double xWaypoint = 0.0;
        private double yWaypoint = 0.0;

        // Orientation du Waypoint en radians
        private double thetaWaypoint = 0.0;

        // Distance entre Ghost et Waypoint
        private double distanceWP = 0.0;

        // ============================================================
        // PARAMETRES GRAPHIQUES
        // ============================================================

        // Pixels par mètre
        private double echelle = 100.0;

        // Taille du Ghost
        private double longueurRobot = 0.40;
        private double largeurRobot = 0.30;

        // Taille graphique de la flèche du Waypoint
        private double longueurFlecheWaypoint = 0.30;

        // ============================================================
        // CONSTRUCTEUR
        // ============================================================

        public RobotPositionInterface()
        {
            InitializeComponent();

            SizeChanged += RobotPositionInterface_SizeChanged;
            Loaded += RobotPositionInterface_Loaded;
        }

        // ============================================================
        // INITIALISATION
        // ============================================================

        private void RobotPositionInterface_Loaded(
            object sender,
            RoutedEventArgs e)
        {
            Dessiner();
        }

        private void RobotPositionInterface_SizeChanged(
            object sender,
            SizeChangedEventArgs e)
        {
            Dessiner();
        }

        // ============================================================
        // FONCTION PUBLIQUE
        // ============================================================

        /// <summary>
        /// Met à jour et affiche le Ghost et le Waypoint.
        ///
        /// xGhost        : position X du Ghost en mètres
        /// yGhost        : position Y du Ghost en mètres
        /// thetaGhost    : orientation du Ghost en radians
        ///
        /// xWaypoint     : position X du Waypoint en mètres
        /// yWaypoint     : position Y du Waypoint en mètres
        /// thetaWaypoint : orientation du Waypoint en radians
        ///
        /// distanceWP    : distance Ghost -> Waypoint en mètres
        /// </summary>
        public void AfficherPosition(
            double xGhost,
            double yGhost,
            double thetaGhost,
            double xWaypoint,
            double yWaypoint,
            double thetaWaypoint,
            double distanceWP)
        {
            this.xGhost = xGhost;
            this.yGhost = yGhost;
            this.thetaGhost = thetaGhost;

            this.xWaypoint = xWaypoint;
            this.yWaypoint = yWaypoint;
            this.thetaWaypoint = thetaWaypoint;

            this.distanceWP = distanceWP;

            Dessiner();
        }

        // ============================================================
        // CONVERSION METRES -> PIXELS
        // ============================================================

        private Point CoordonneesVersCanvas(
            double x,
            double y)
        {
            double origineX =
                canvasRobot.ActualWidth / 2.0;

            double origineY =
                canvasRobot.ActualHeight / 2.0;

            double pixelX =
                origineX + x * echelle;

            // Repère mathématique :
            // Y positif vers le haut.
            //
            // WPF :
            // Y positif vers le bas.

            double pixelY =
                origineY - y * echelle;

            return new Point(
                pixelX,
                pixelY);
        }

        // ============================================================
        // DESSIN GENERAL
        // ============================================================

        private void Dessiner()
        {
            if (canvasRobot.ActualWidth <= 0 ||
                canvasRobot.ActualHeight <= 0)
            {
                return;
            }

            canvasRobot.Children.Clear();

            DessinerAxes();
            DessinerLiaisonGhostWaypoint();
            DessinerWaypoint();
            DessinerGhost();
        }

        // ============================================================
        // AXES
        // ============================================================

        private void DessinerAxes()
        {
            double largeur =
                canvasRobot.ActualWidth;

            double hauteur =
                canvasRobot.ActualHeight;

            double origineX =
                largeur / 2.0;

            double origineY =
                hauteur / 2.0;

            // --------------------------------------------------------
            // AXE X
            // --------------------------------------------------------

            Line axeX = new Line
            {
                X1 = 0,
                Y1 = origineY,

                X2 = largeur,
                Y2 = origineY,

                Stroke = Brushes.LightGray,
                StrokeThickness = 1
            };

            canvasRobot.Children.Add(axeX);

            // --------------------------------------------------------
            // AXE Y
            // --------------------------------------------------------

            Line axeY = new Line
            {
                X1 = origineX,
                Y1 = 0,

                X2 = origineX,
                Y2 = hauteur,

                Stroke = Brushes.LightGray,
                StrokeThickness = 1
            };

            canvasRobot.Children.Add(axeY);

            // --------------------------------------------------------
            // FLECHE X
            // --------------------------------------------------------

            Polygon flecheX = new Polygon
            {
                Fill = Brushes.Gray,

                Points = new PointCollection
                {
                    new Point(
                        largeur - 5,
                        origineY),

                    new Point(
                        largeur - 15,
                        origineY - 5),

                    new Point(
                        largeur - 15,
                        origineY + 5)
                }
            };

            canvasRobot.Children.Add(flecheX);

            // --------------------------------------------------------
            // FLECHE Y
            // --------------------------------------------------------

            Polygon flecheY = new Polygon
            {
                Fill = Brushes.Gray,

                Points = new PointCollection
                {
                    new Point(
                        origineX,
                        5),

                    new Point(
                        origineX - 5,
                        15),

                    new Point(
                        origineX + 5,
                        15)
                }
            };

            canvasRobot.Children.Add(flecheY);

            // --------------------------------------------------------
            // LABEL X
            // --------------------------------------------------------

            TextBlock texteX = new TextBlock
            {
                Text = "X",
                FontWeight = FontWeights.Bold,
                Foreground = Brushes.Gray
            };

            Canvas.SetLeft(
                texteX,
                largeur - 25);

            Canvas.SetTop(
                texteX,
                origineY + 5);

            canvasRobot.Children.Add(texteX);

            // --------------------------------------------------------
            // LABEL Y
            // --------------------------------------------------------

            TextBlock texteY = new TextBlock
            {
                Text = "Y",
                FontWeight = FontWeights.Bold,
                Foreground = Brushes.Gray
            };

            Canvas.SetLeft(
                texteY,
                origineX + 5);

            Canvas.SetTop(
                texteY,
                5);

            canvasRobot.Children.Add(texteY);
        }

        // ============================================================
        // LIGNE GHOST -> WAYPOINT
        // ============================================================

        private void DessinerLiaisonGhostWaypoint()
        {
            Point ghost =
                CoordonneesVersCanvas(
                    xGhost,
                    yGhost);

            Point waypoint =
                CoordonneesVersCanvas(
                    xWaypoint,
                    yWaypoint);

            Line ligne = new Line
            {
                X1 = ghost.X,
                Y1 = ghost.Y,

                X2 = waypoint.X,
                Y2 = waypoint.Y,

                Stroke = Brushes.OrangeRed,
                StrokeThickness = 2,

                StrokeDashArray =
                    new DoubleCollection
                    {
                        5,
                        3
                    }
            };

            canvasRobot.Children.Add(ligne);
        }

        // ============================================================
        // WAYPOINT
        // ============================================================

        private void DessinerWaypoint()
        {
            Point centre =
                CoordonneesVersCanvas(
                    xWaypoint,
                    yWaypoint);

            // --------------------------------------------------------
            // CERCLE DU WAYPOINT
            // --------------------------------------------------------

            Ellipse cercle = new Ellipse
            {
                Width = 18,
                Height = 18,

                Fill = Brushes.Transparent,
                Stroke = Brushes.Red,
                StrokeThickness = 3
            };

            Canvas.SetLeft(
                cercle,
                centre.X - cercle.Width / 2);

            Canvas.SetTop(
                cercle,
                centre.Y - cercle.Height / 2);

            canvasRobot.Children.Add(cercle);

            // --------------------------------------------------------
            // CROIX HORIZONTALE
            // --------------------------------------------------------

            Line ligneX = new Line
            {
                X1 = centre.X - 10,
                Y1 = centre.Y,

                X2 = centre.X + 10,
                Y2 = centre.Y,

                Stroke = Brushes.Red,
                StrokeThickness = 2
            };

            canvasRobot.Children.Add(ligneX);

            // --------------------------------------------------------
            // CROIX VERTICALE
            // --------------------------------------------------------

            Line ligneY = new Line
            {
                X1 = centre.X,
                Y1 = centre.Y - 10,

                X2 = centre.X,
                Y2 = centre.Y + 10,

                Stroke = Brushes.Red,
                StrokeThickness = 2
            };

            canvasRobot.Children.Add(ligneY);

            // --------------------------------------------------------
            // DIRECTION DU WAYPOINT
            // --------------------------------------------------------

            double longueur =
                longueurFlecheWaypoint * echelle;

            double cos =
                Math.Cos(thetaWaypoint);

            double sin =
                Math.Sin(thetaWaypoint);

            // Pointe de la direction
            Point pointe = new Point(
                centre.X +
                cos * longueur,

                centre.Y -
                sin * longueur
            );

            // Ligne de direction
            Line direction = new Line
            {
                X1 = centre.X,
                Y1 = centre.Y,

                X2 = pointe.X,
                Y2 = pointe.Y,

                Stroke = Brushes.Red,
                StrokeThickness = 3
            };

            canvasRobot.Children.Add(direction);

            // --------------------------------------------------------
            // TEXTE WAYPOINT
            // --------------------------------------------------------

            double angleDegres =
                thetaWaypoint *
                180.0 /
                Math.PI;

            TextBlock texte = new TextBlock
            {
                Text =
                    $"WP\n" +
                    $"x = {xWaypoint:F2} m\n" +
                    $"y = {yWaypoint:F2} m\n" +
                    $"θ = {angleDegres:F1}°\n" +
                    $"Distance = {distanceWP:F2} m",

                Foreground = Brushes.Red,
                FontWeight = FontWeights.Bold
            };

            Canvas.SetLeft(
                texte,
                centre.X + 15);

            Canvas.SetTop(
                texte,
                centre.Y - 60);

            canvasRobot.Children.Add(texte);
        }

        // ============================================================
        // GHOST / ROBOT
        // ============================================================

        private void DessinerGhost()
        {
            Point centre =
                CoordonneesVersCanvas(
                    xGhost,
                    yGhost);

            double longueur =
                longueurRobot * echelle;

            double largeur =
                largeurRobot * echelle;

            // ========================================================
            // ORIENTATION DU ROBOT
            // ========================================================

            double cos =
                Math.Cos(thetaGhost);

            double sin =
                Math.Sin(thetaGhost);

            // ========================================================
            // POINTE DU ROBOT
            // ========================================================

            Point pointe = new Point(
                centre.X +
                cos * longueur / 2.0,

                centre.Y -
                sin * longueur / 2.0
            );

            // ========================================================
            // ARRIERE GAUCHE
            // ========================================================

            Point arriereGauche = new Point(
                centre.X
                - cos * longueur / 2.0
                + sin * largeur / 2.0,

                centre.Y
                + sin * longueur / 2.0
                + cos * largeur / 2.0
            );

            // ========================================================
            // ARRIERE DROIT
            // ========================================================

            Point arriereDroit = new Point(
                centre.X
                - cos * longueur / 2.0
                - sin * largeur / 2.0,

                centre.Y
                + sin * longueur / 2.0
                - cos * largeur / 2.0
            );

            // ========================================================
            // CORPS DU ROBOT
            // ========================================================

            Polygon robot = new Polygon
            {
                Fill = Brushes.DodgerBlue,
                Stroke = Brushes.DarkBlue,
                StrokeThickness = 2,

                Points = new PointCollection
                {
                    pointe,
                    arriereGauche,
                    arriereDroit
                }
            };

            canvasRobot.Children.Add(robot);

            // ========================================================
            // CENTRE DU ROBOT
            // ========================================================

            Ellipse centreRobot = new Ellipse
            {
                Width = 8,
                Height = 8,

                Fill = Brushes.White,
                Stroke = Brushes.DarkBlue,
                StrokeThickness = 2
            };

            Canvas.SetLeft(
                centreRobot,
                centre.X -
                centreRobot.Width / 2);

            Canvas.SetTop(
                centreRobot,
                centre.Y -
                centreRobot.Height / 2);

            canvasRobot.Children.Add(centreRobot);

            // ========================================================
            // TEXTE GHOST
            // ========================================================

            double angleDegres =
                thetaGhost *
                180.0 /
                Math.PI;

            TextBlock texte = new TextBlock
            {
                Text =
                    $"Ghost\n" +
                    $"x = {xGhost:F2} m\n" +
                    $"y = {yGhost:F2} m\n" +
                    $"θ = {angleDegres:F1}°",

                Foreground = Brushes.DarkBlue,
                FontWeight = FontWeights.Bold
            };

            Canvas.SetLeft(
                texte,
                centre.X + 15);

            Canvas.SetTop(
                texte,
                centre.Y + 10);

            canvasRobot.Children.Add(texte);
        }
    }
}*/

/*using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Shapes;

namespace Robot_interface_fath_bertin
{
    public partial class RobotPositionInterface : UserControl
    {
        // ============================================================
        // DONNEES A AFFICHER
        // ============================================================

        // Position du Ghost en mètres
        private double xGhost = 0.0;
        private double yGhost = 0.0;

        // Orientation du Ghost en radians
        private double thetaGhost = 0.0;

        // Position du Waypoint en mètres
        private double xWaypoint = 0.0;
        private double yWaypoint = 0.0;

        // ============================================================
        // PARAMETRES GRAPHIQUES
        // ============================================================

        // Pixels par mètre
        private double echelle = 100.0;

        // Taille graphique du robot
        private double longueurRobot = 0.40;
        private double largeurRobot = 0.30;

        // ============================================================
        // CONSTRUCTEUR
        // ============================================================

        public RobotPositionInterface()
        {
            InitializeComponent();

            SizeChanged += RobotPositionInterface_SizeChanged;
            Loaded += RobotPositionInterface_Loaded;
        }

        // ============================================================
        // INITIALISATION
        // ============================================================

        private void RobotPositionInterface_Loaded(
            object sender,
            RoutedEventArgs e)
        {
            Dessiner();
        }

        private void RobotPositionInterface_SizeChanged(
            object sender,
            SizeChangedEventArgs e)
        {
            Dessiner();
        }

        // ============================================================
        // FONCTION PUBLIQUE D'AFFICHAGE
        // ============================================================

        /// <summary>
        /// Affiche le Ghost et le Waypoint.
        ///
        /// xGhost, yGhost : position du Ghost en mètres
        /// thetaGhost     : orientation du Ghost en radians
        /// xWaypoint      : position X du Waypoint en mètres
        /// yWaypoint      : position Y du Waypoint en mètres
        ///
        /// IMPORTANT :
        /// Cette fonction ne fait AUCUN calcul de trajectoire.
        /// Elle affiche uniquement les valeurs fournies.
        /// </summary>
        public void AfficherPosition(
            double xGhost,
            double yGhost,
            double thetaGhost,
            double xWaypoint,
            double yWaypoint)
        {
            this.xGhost = xGhost;
            this.yGhost = yGhost;
            this.thetaGhost = thetaGhost;

            this.xWaypoint = xWaypoint;
            this.yWaypoint = yWaypoint;

            Dessiner();
        }

        // ============================================================
        // CONVERSION METRES -> PIXELS
        // ============================================================

        private Point CoordonneesVersCanvas(
            double x,
            double y)
        {
            double origineX =
                canvasRobot.ActualWidth / 2.0;

            double origineY =
                canvasRobot.ActualHeight / 2.0;

            double pixelX =
                origineX + x * echelle;

            // Repère mathématique :
            // Y positif vers le haut.
            //
            // WPF :
            // Y positif vers le bas.
            //
            // On inverse donc Y.

            double pixelY =
                origineY - y * echelle;

            return new Point(
                pixelX,
                pixelY);
        }

        // ============================================================
        // DESSIN GENERAL
        // ============================================================

        private void Dessiner()
        {
            if (canvasRobot.ActualWidth <= 0 ||
                canvasRobot.ActualHeight <= 0)
            {
                return;
            }

            canvasRobot.Children.Clear();

            DessinerAxes();
            DessinerLiaisonGhostWaypoint();
            DessinerWaypoint();
            DessinerGhost();
        }

        // ============================================================
        // AXES
        // ============================================================

        private void DessinerAxes()
        {
            double largeur =
                canvasRobot.ActualWidth;

            double hauteur =
                canvasRobot.ActualHeight;

            double origineX =
                largeur / 2.0;

            double origineY =
                hauteur / 2.0;

            // --------------------------------------------------------
            // AXE X
            // --------------------------------------------------------

            Line axeX = new Line
            {
                X1 = 0,
                Y1 = origineY,

                X2 = largeur,
                Y2 = origineY,

                Stroke = Brushes.LightGray,
                StrokeThickness = 1
            };

            canvasRobot.Children.Add(axeX);

            // --------------------------------------------------------
            // AXE Y
            // --------------------------------------------------------

            Line axeY = new Line
            {
                X1 = origineX,
                Y1 = 0,

                X2 = origineX,
                Y2 = hauteur,

                Stroke = Brushes.LightGray,
                StrokeThickness = 1
            };

            canvasRobot.Children.Add(axeY);

            // --------------------------------------------------------
            // FLECHE X
            // --------------------------------------------------------

            Polygon flecheX = new Polygon
            {
                Fill = Brushes.Gray,

                Points = new PointCollection
                {
                    new Point(
                        largeur - 5,
                        origineY),

                    new Point(
                        largeur - 15,
                        origineY - 5),

                    new Point(
                        largeur - 15,
                        origineY + 5)
                }
            };

            canvasRobot.Children.Add(flecheX);

            // --------------------------------------------------------
            // FLECHE Y
            // --------------------------------------------------------

            Polygon flecheY = new Polygon
            {
                Fill = Brushes.Gray,

                Points = new PointCollection
                {
                    new Point(
                        origineX,
                        5),

                    new Point(
                        origineX - 5,
                        15),

                    new Point(
                        origineX + 5,
                        15)
                }
            };

            canvasRobot.Children.Add(flecheY);

            // --------------------------------------------------------
            // LABEL X
            // --------------------------------------------------------

            TextBlock texteX = new TextBlock
            {
                Text = "X",
                FontWeight = FontWeights.Bold,
                Foreground = Brushes.Gray
            };

            Canvas.SetLeft(
                texteX,
                largeur - 25);

            Canvas.SetTop(
                texteX,
                origineY + 5);

            canvasRobot.Children.Add(texteX);

            // --------------------------------------------------------
            // LABEL Y
            // --------------------------------------------------------

            TextBlock texteY = new TextBlock
            {
                Text = "Y",
                FontWeight = FontWeights.Bold,
                Foreground = Brushes.Gray
            };

            Canvas.SetLeft(
                texteY,
                origineX + 5);

            Canvas.SetTop(
                texteY,
                5);

            canvasRobot.Children.Add(texteY);
        }

        // ============================================================
        // LIGNE GHOST -> WAYPOINT
        // ============================================================

        private void DessinerLiaisonGhostWaypoint()
        {
            Point ghost =
                CoordonneesVersCanvas(
                    xGhost,
                    yGhost);

            Point waypoint =
                CoordonneesVersCanvas(
                    xWaypoint,
                    yWaypoint);

            Line ligne = new Line
            {
                X1 = ghost.X,
                Y1 = ghost.Y,

                X2 = waypoint.X,
                Y2 = waypoint.Y,

                Stroke = Brushes.OrangeRed,
                StrokeThickness = 2,

                StrokeDashArray =
                    new DoubleCollection
                    {
                        5,
                        3
                    }
            };

            canvasRobot.Children.Add(ligne);
        }

        // ============================================================
        // WAYPOINT
        // ============================================================

        private void DessinerWaypoint()
        {
            Point centre =
                CoordonneesVersCanvas(
                    xWaypoint,
                    yWaypoint);

            // --------------------------------------------------------
            // CERCLE
            // --------------------------------------------------------

            Ellipse cercle = new Ellipse
            {
                Width = 18,
                Height = 18,

                Fill = Brushes.Transparent,
                Stroke = Brushes.Red,
                StrokeThickness = 3
            };

            Canvas.SetLeft(
                cercle,
                centre.X - cercle.Width / 2);

            Canvas.SetTop(
                cercle,
                centre.Y - cercle.Height / 2);

            canvasRobot.Children.Add(cercle);

            // --------------------------------------------------------
            // CROIX HORIZONTALE
            // --------------------------------------------------------

            Line ligneX = new Line
            {
                X1 = centre.X - 10,
                Y1 = centre.Y,

                X2 = centre.X + 10,
                Y2 = centre.Y,

                Stroke = Brushes.Red,
                StrokeThickness = 2
            };

            canvasRobot.Children.Add(ligneX);

            // --------------------------------------------------------
            // CROIX VERTICALE
            // --------------------------------------------------------

            Line ligneY = new Line
            {
                X1 = centre.X,
                Y1 = centre.Y - 10,

                X2 = centre.X,
                Y2 = centre.Y + 10,

                Stroke = Brushes.Red,
                StrokeThickness = 2
            };

            canvasRobot.Children.Add(ligneY);

            // --------------------------------------------------------
            // TEXTE WAYPOINT
            // --------------------------------------------------------

            TextBlock texte = new TextBlock
            {
                Text =
                    $"WP ({xWaypoint:F2} ; {yWaypoint:F2})",

                Foreground = Brushes.Red,
                FontWeight = FontWeights.Bold
            };

            Canvas.SetLeft(
                texte,
                centre.X + 12);

            Canvas.SetTop(
                texte,
                centre.Y - 25);

            canvasRobot.Children.Add(texte);
        }

        // ============================================================
        // GHOST / ROBOT
        // ============================================================

        private void DessinerGhost()
        {
            Point centre =
                CoordonneesVersCanvas(
                    xGhost,
                    yGhost);

            double longueur =
                longueurRobot * echelle;

            double largeur =
                largeurRobot * echelle;

            // ========================================================
            // ORIENTATION
            // ========================================================

            double cos =
                Math.Cos(thetaGhost);

            double sin =
                Math.Sin(thetaGhost);

            // ========================================================
            // POINTE DU ROBOT
            // ========================================================

            Point pointe = new Point(
                centre.X +
                cos * longueur / 2.0,

                centre.Y -
                sin * longueur / 2.0
            );

            // ========================================================
            // ARRIERE GAUCHE
            // ========================================================

            Point arriereGauche = new Point(
                centre.X
                - cos * longueur / 2.0
                + sin * largeur / 2.0,

                centre.Y
                + sin * longueur / 2.0
                + cos * largeur / 2.0
            );

            // ========================================================
            // ARRIERE DROIT
            // ========================================================

            Point arriereDroit = new Point(
                centre.X
                - cos * longueur / 2.0
                - sin * largeur / 2.0,

                centre.Y
                + sin * longueur / 2.0
                - cos * largeur / 2.0
            );

            // ========================================================
            // CORPS DU ROBOT
            // ========================================================

            Polygon robot = new Polygon
            {
                Fill = Brushes.DodgerBlue,
                Stroke = Brushes.DarkBlue,
                StrokeThickness = 2,

                Points = new PointCollection
                {
                    pointe,
                    arriereGauche,
                    arriereDroit
                }
            };

            canvasRobot.Children.Add(robot);

            // ========================================================
            // CENTRE DU ROBOT
            // ========================================================

            Ellipse centreRobot = new Ellipse
            {
                Width = 8,
                Height = 8,

                Fill = Brushes.White,
                Stroke = Brushes.DarkBlue,
                StrokeThickness = 2
            };

            Canvas.SetLeft(
                centreRobot,
                centre.X -
                centreRobot.Width / 2);

            Canvas.SetTop(
                centreRobot,
                centre.Y -
                centreRobot.Height / 2);

            canvasRobot.Children.Add(centreRobot);

            // ========================================================
            // TEXTE GHOST
            // ========================================================

            double angleDegres =
                thetaGhost *
                180.0 /
                Math.PI;

            TextBlock texte = new TextBlock
            {
                Text =
                    $"Ghost\n" +
                    $"x = {xGhost:F2} m\n" +
                    $"y = {yGhost:F2} m\n" +
                    $"θ = {angleDegres:F1}°",

                Foreground = Brushes.DarkBlue,
                FontWeight = FontWeights.Bold
            };

            Canvas.SetLeft(
                texte,
                centre.X + 15);

            Canvas.SetTop(
                texte,
                centre.Y + 10);

            canvasRobot.Children.Add(texte);
        }
    }
}
*/