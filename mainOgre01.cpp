#include "Ogre\ExampleApplication.h"

class FrameListenerTurrets : public Ogre::FrameListener {

private:
	Ogre::SceneNode* nave;
	Ogre::SceneNode** turrets;

public:
	FrameListenerTurrets(Ogre::SceneNode* node, Ogre::SceneNode** nodes){
		nave = node;
		turrets = nodes;
	}

	bool frameStarted(const Ogre::FrameEvent &evt){
		Ogre::String count;
		for (int i = 0; i < 8; i++)
		{
			count = Ogre::StringConverter::toString(i);
			turrets[i]->getChild("Platillo" + count)->yaw(Ogre::Degree(200.0f * evt.timeSinceLastFrame));
		}
		return true;
	}
};	

class FrameListenerHelices : public Ogre::FrameListener {

private:
	Ogre::SceneNode** helices;

public:
	FrameListenerHelices(Ogre::SceneNode** nodes){
		helices = nodes;
	}

	bool frameStarted(const Ogre::FrameEvent &evt){
		for (int i = 0; i < 2; i++)
		{
			helices[i]->rotate(Quaternion(Degree(90 * evt.timeSinceLastFrame), Vector3::UNIT_Y));
		}
		return true;
	}
};


class FrameListenerNave : public Ogre::FrameListener {

private:
	Ogre::SceneNode* nave, *camara;
	OIS::InputManager* _man;
	OIS::Keyboard* _key;
	float grados;

public:
	FrameListenerNave(Ogre::SceneNode* node, RenderWindow* win, Ogre::SceneNode* Cam){
		// Interaccion teclado
		size_t windowHnd = 0;
		std::stringstream windowsHndStr;
		win->getCustomAttribute("WINDOW", &windowHnd);
		windowsHndStr << windowHnd;
		OIS::ParamList pl;
		pl.insert(std::make_pair(std::string("WINDOW"), windowsHndStr.str()));
		_man = OIS::InputManager::createInputSystem(pl);
		_key = static_cast<OIS::Keyboard*>(_man->createInputObject(OIS::OISKeyboard,false));
		
		nave = node;
		camara = Cam;
		grados = 0.0f;
	}

	~FrameListenerNave(){
		_man->destroyInputObject(_key);
		OIS::InputManager::destroyInputSystem(_man);
	}

	bool frameStarted(const Ogre::FrameEvent &evt){
		float movSpeed = 2500.0f;
		_key->capture();
		
		if (_key->isKeyDown(OIS::KC_ESCAPE))
			return false;

		Ogre::Vector3 t (0,0,0);
		Ogre::Quaternion q (Degree(0), Vector3::ZERO);
		Ogre::Quaternion qc (Degree(0), Vector3::ZERO);

		if (_key->isKeyDown(OIS::KC_W))
			t += Ogre::Vector3(0,0,-movSpeed);

		if(_key->isKeyDown(OIS::KC_D) || _key->isKeyDown(OIS::KC_A)){
			if (_key->isKeyDown(OIS::KC_A)){
				t += Ogre::Vector3(-movSpeed,0,0);
				if (grados > -45)
				{
					grados -= 90 * evt.timeSinceLastFrame;
					q = q * Quaternion(Degree(-90 * evt.timeSinceLastFrame), Vector3::UNIT_X);
				}
			}
			if (_key->isKeyDown(OIS::KC_D)){
				t += Ogre::Vector3(movSpeed,0,0);
				if (grados < 45)
				{
					grados += 90 * evt.timeSinceLastFrame;
					q = q * Quaternion(Degree(90 * evt.timeSinceLastFrame), Vector3::UNIT_X);
				}
			}
		} else
		{
			if (grados > 0.1f)
			{
				grados -= 90 * evt.timeSinceLastFrame;
				q = q * Quaternion(Degree(-90 * evt.timeSinceLastFrame), Vector3::UNIT_X);
			} else if (grados < -0.001f){
				grados += 90 * evt.timeSinceLastFrame;
				q = q * Quaternion(Degree(90 * evt.timeSinceLastFrame), Vector3::UNIT_X);
			}
		}
		if (_key->isKeyDown(OIS::KC_LEFT))
			qc = Quaternion(Degree(90 * evt.timeSinceLastFrame), Vector3::UNIT_Y);

		if (_key->isKeyDown(OIS::KC_RIGHT))
			qc = Quaternion(Degree(-90 * evt.timeSinceLastFrame), Vector3::UNIT_Y);

		if (_key->isKeyDown(OIS::KC_UP))
			t += Ogre::Vector3(0,movSpeed,0);

		if (_key->isKeyDown(OIS::KC_DOWN))
			t += Ogre::Vector3(0,-movSpeed,0);

		nave->rotate(q);
		camara->translate(t * evt.timeSinceLastFrame, camara->TS_LOCAL);
		camara->rotate(qc);
		

		// Limites del cuarto
		Vector3 pos = camara->getPosition();
		if (pos.x > 1750)
		{
			if(pos.z > -18550){
				pos.x = 1740;
			} else if (pos.x >26684){
				pos.x = 26683;
			} 
		} else if(pos.x < -1750) {
			if (pos.z > -18550)
			{
				pos.x = -1740;
			} else if (pos.x < -26684){
				pos.x = -26683;
			}
		}

		if (pos.z > 0.1f)
		{
			pos.z = 0;
		} else if(pos.x < -1800 || pos.x > 1800 ) {
			if (pos.z < -23500)
			{
				pos.z = -23490;
			} else if (pos.z > -18400){
				pos.z = -18410;
			}
		} else if(pos.z < -23500)
		{
			pos.z=-23490;
		}

		if (pos.y > 1700)
		{
			pos.y = 1690;
		} else if(pos.y < -1200) {
			pos.y = -1190;
		}

		camara->setPosition(pos);
		return true;
	}
};

class StarWars : public ExampleApplication
{
public:
	int cantTriangulo;

	class Vertex{
		public:
		Vector3 posi;
		Vector3 normal;
			Vertex() {
				posi = 0;
				normal = 0;
			}

			 Vertex(Vector3 pos, Vector3 norm) {
				posi = pos;
				normal = norm;
			}
	};

	class Triangle {
		public:
		Vertex* first;
		Vertex* second;
		Vertex* last;
			Triangle() {
				first = new Vertex();
				second =  new Vertex();
				last = new Vertex();
			}
			Triangle(Vertex* a, Vertex* b, Vertex* c) {
				first = a;
				second = b;
				last = c;
			}
	};

	std::vector<Vertex*> posiList;
	std::vector<Triangle*> triangulos;

	Ogre::SceneNode* nave, *camera_node, *turrets[8], *shots[8], *helices[2];
	Ogre::FrameListener* FrameListenerTurrets01;
	Ogre::FrameListener* FrameListenerNave01;
	Ogre::FrameListener* FrameListenerHelices01;

	Ogre::Vector3 getNormal(Ogre::Vector3 vec1, Ogre::Vector3 vec2, Ogre::Vector3 vec3) {
		Vector3 a = vec2 - vec1;
		Vector3 b = vec3 - vec1;

		Vector3 normal = a.crossProduct(b);
		normal.normalise();
		return normal;
	}

	void calcularNormales() {
		cantTriangulo = 42;
		//Primer plano
		posiList.emplace_back(new Vertex(Ogre::Vector3(-10.0, 30.0, 0.0), Ogre::Vector3(0.0, 0.0, 0.0)));
		posiList.emplace_back(new Vertex(Ogre::Vector3(-7.5, 20.0, -100.0), Ogre::Vector3(0.0, 0.0, 0.0)));
		posiList.emplace_back(new Vertex(Ogre::Vector3(-30.0, 10.0, 0.0), Ogre::Vector3(0.0, 0.0, 0.0)));
		posiList.emplace_back(new Vertex(Ogre::Vector3(-20.0, 7.5, -100.0), Ogre::Vector3(0.0, 0.0, 0.0)));
		posiList.emplace_back(new Vertex(Ogre::Vector3(-30.0, -10.0, 0.0), Ogre::Vector3(0.0, 0.0, 0.0)));
		posiList.emplace_back(new Vertex(Ogre::Vector3(-20.0, -7.5, -100.0), Ogre::Vector3(0.0, 0.0, 0.0)));
		posiList.emplace_back(new Vertex(Ogre::Vector3( -10.0, -30.0, 0.0), Ogre::Vector3(0.0, 0.0, 0.0)));
		posiList.emplace_back(new Vertex(Ogre::Vector3( -7.5, -20.0, -100.0), Ogre::Vector3(0.0, 0.0, 0.0)));
		posiList.emplace_back(new Vertex(Ogre::Vector3( 10.0, -30.0,0.0), Ogre::Vector3(0.0, 0.0, 0.0)));
		posiList.emplace_back(new Vertex(Ogre::Vector3( 7.5, -20.0, -100.0), Ogre::Vector3(0.0, 0.0, 0.0)));
		posiList.emplace_back(new Vertex(Ogre::Vector3( 30.0, -10.0,0.0), Ogre::Vector3(0.0, 0.0, 0.0)));
		posiList.emplace_back(new Vertex(Ogre::Vector3( 20.0, -7.5, -100.0), Ogre::Vector3(0.0, 0.0, 0.0)));
		posiList.emplace_back(new Vertex(Ogre::Vector3( 30.0, 10.0,0.0), Ogre::Vector3(0.0, 0.0, 0.0)));
		posiList.emplace_back(new Vertex(Ogre::Vector3( 20.0, 7.5, -100.0), Ogre::Vector3(0.0, 0.0, 0.0)));
		posiList.emplace_back(new Vertex(Ogre::Vector3( 10.0, 30.0,0.0), Ogre::Vector3(0.0, 0.0, 0.0)));
		posiList.emplace_back(new Vertex(Ogre::Vector3( 7.5, 20.0, -100.0), Ogre::Vector3(0.0, 0.0, 0.0)));

		//indice 16
		posiList.emplace_back(new Vertex(Ogre::Vector3(-3.75, 10.0, -200.0), Ogre::Vector3(0.0, 0.0, 0.0)));
		posiList.emplace_back(new Vertex(Ogre::Vector3( -10.0, 3.75, -200.0), Ogre::Vector3(0.0, 0.0, 0.0)));
		posiList.emplace_back(new Vertex(Ogre::Vector3( -10.0, -3.75, -200.0), Ogre::Vector3(0.0, 0.0, 0.0)));
		posiList.emplace_back(new Vertex(Ogre::Vector3( -3.75, -10.0, -200.0), Ogre::Vector3(0.0, 0.0, 0.0)));
		posiList.emplace_back(new Vertex(Ogre::Vector3( 3.75, -10.0, -200.0), Ogre::Vector3(0.0, 0.0, 0.0)));
		posiList.emplace_back(new Vertex(Ogre::Vector3( 10.0, -3.75, -200.0), Ogre::Vector3(0.0, 0.0, 0.0)));
		posiList.emplace_back(new Vertex(Ogre::Vector3( 10.0, 3.75, -200.0), Ogre::Vector3(0.0, 0.0, 0.0)));
		posiList.emplace_back(new Vertex(Ogre::Vector3( 3.75, 10.0, -200.0), Ogre::Vector3(0.0, 0.0, 0.0)));
		//indice23
		posiList.emplace_back(new Vertex(Ogre::Vector3(30.0, 50.0, -200.0), Ogre::Vector3(0.0, 0.0, 0.0)));
		posiList.emplace_back(new Vertex(Ogre::Vector3(350.0, 50.0, -200.0), Ogre::Vector3(0.0, 0.0, 0.0)));
		posiList.emplace_back(new Vertex(Ogre::Vector3(0.0, 150.0, -150.0), Ogre::Vector3(0.0, 0.0, 0.0)));
		posiList.emplace_back(new Vertex(Ogre::Vector3( -100.0, 150.0, -150.0), Ogre::Vector3(0.0, 0.0, 0.0)));
		posiList.emplace_back(new Vertex(Ogre::Vector3( -100.0, 50.0, -200.0), Ogre::Vector3(0.0, 0.0, 0.0)));
		posiList.emplace_back(new Vertex(Ogre::Vector3( -100.0, -50.0, -200.0), Ogre::Vector3(0.0, 0.0, 0.0)));
		posiList.emplace_back(new Vertex(Ogre::Vector3( -100.0, -150.0, -150.0), Ogre::Vector3(0.0, 0.0, 0.0)));
		posiList.emplace_back(new Vertex(Ogre::Vector3( 0.0, -150.0, -150.0), Ogre::Vector3(0.0, 0.0, 0.0)));
		posiList.emplace_back(new Vertex(Ogre::Vector3( 350.0, -50.0, -200.0), Ogre::Vector3(0.0, 0.0, 0.0)));
		posiList.emplace_back(new Vertex(Ogre::Vector3(30.0, -50.0, -200.0), Ogre::Vector3(0.0, 0.0, 0.0)));

		//Se crean todos los triangulos.
		triangulos.emplace_back(new Triangle(posiList.at(0), posiList.at(1), posiList.at(2)));
		triangulos.emplace_back(new Triangle(posiList.at(2), posiList.at(1), posiList.at(3)));
		triangulos.emplace_back(new Triangle(posiList.at(2), posiList.at(3), posiList.at(4)));
		triangulos.emplace_back(new Triangle(posiList.at(4), posiList.at(3), posiList.at(5)));
		triangulos.emplace_back(new Triangle(posiList.at(4), posiList.at(5), posiList.at(6)));
		triangulos.emplace_back(new Triangle(posiList.at(6), posiList.at(5), posiList.at(7)));
		triangulos.emplace_back(new Triangle(posiList.at(6), posiList.at(7), posiList.at(8)));
		triangulos.emplace_back(new Triangle(posiList.at(8), posiList.at(7), posiList.at(9)));
		triangulos.emplace_back(new Triangle(posiList.at(8), posiList.at(9), posiList.at(10)));
		triangulos.emplace_back(new Triangle(posiList.at(10), posiList.at(9), posiList.at(11)));
		triangulos.emplace_back(new Triangle(posiList.at(10), posiList.at(11), posiList.at(12)));
		triangulos.emplace_back(new Triangle(posiList.at(11), posiList.at(13), posiList.at(12)));
		triangulos.emplace_back(new Triangle(posiList.at(10), posiList.at(11), posiList.at(12)));
		triangulos.emplace_back(new Triangle(posiList.at(11), posiList.at(13), posiList.at(12)));
		triangulos.emplace_back(new Triangle(posiList.at(13), posiList.at(14), posiList.at(12)));
		triangulos.emplace_back(new Triangle(posiList.at(14), posiList.at(13), posiList.at(15)));
		triangulos.emplace_back(new Triangle(posiList.at(14), posiList.at(15), posiList.at(0)));
		triangulos.emplace_back(new Triangle(posiList.at(0), posiList.at(15), posiList.at(1)));

		triangulos.emplace_back(new Triangle(posiList.at(1), posiList.at(16), posiList.at(3)));
		triangulos.emplace_back(new Triangle(posiList.at(3), posiList.at(16), posiList.at(17)));
		triangulos.emplace_back(new Triangle(posiList.at(3), posiList.at(17), posiList.at(5)));
		triangulos.emplace_back(new Triangle(posiList.at(5), posiList.at(17), posiList.at(18)));
		triangulos.emplace_back(new Triangle(posiList.at(5), posiList.at(18), posiList.at(7)));
		triangulos.emplace_back(new Triangle(posiList.at(7), posiList.at(18), posiList.at(19)));
		triangulos.emplace_back(new Triangle(posiList.at(7), posiList.at(19), posiList.at(9)));
		triangulos.emplace_back(new Triangle(posiList.at(9), posiList.at(19), posiList.at(20)));
		triangulos.emplace_back(new Triangle(posiList.at(9), posiList.at(20), posiList.at(11)));
		triangulos.emplace_back(new Triangle(posiList.at(11), posiList.at(20), posiList.at(21)));
		triangulos.emplace_back(new Triangle(posiList.at(11), posiList.at(21), posiList.at(13)));
		triangulos.emplace_back(new Triangle(posiList.at(13), posiList.at(21), posiList.at(22)));
		triangulos.emplace_back(new Triangle(posiList.at(13), posiList.at(22), posiList.at(15)));
		triangulos.emplace_back(new Triangle(posiList.at(15), posiList.at(22), posiList.at(23)));
		triangulos.emplace_back(new Triangle(posiList.at(15), posiList.at(23), posiList.at(1)));
		triangulos.emplace_back(new Triangle(posiList.at(1), posiList.at(23), posiList.at(16)));

		//triangulos ala
		triangulos.emplace_back(new Triangle(posiList.at(26), posiList.at(24), posiList.at(25)));
		triangulos.emplace_back(new Triangle(posiList.at(27), posiList.at(24), posiList.at(26)));
		triangulos.emplace_back(new Triangle(posiList.at(27), posiList.at(28), posiList.at(24)));
		triangulos.emplace_back(new Triangle(posiList.at(24), posiList.at(28), posiList.at(33)));
		triangulos.emplace_back(new Triangle(posiList.at(28), posiList.at(29), posiList.at(33)));
		triangulos.emplace_back(new Triangle(posiList.at(29), posiList.at(30), posiList.at(33)));
		triangulos.emplace_back(new Triangle(posiList.at(33), posiList.at(30), posiList.at(31)));
		triangulos.emplace_back(new Triangle(posiList.at(33), posiList.at(31), posiList.at(32)));

		//triangulos ala fuera


		//Se calculan todas las normales.
		for(int i = 0; i < cantTriangulo; i++) {
			Triangle* t = triangulos.at(i);
			t->first->normal += getNormal(t->first->posi, t->second->posi, t->last->posi);
			t->first->normal.normalise();
			t->second->normal += getNormal(t->second->posi, t->last->posi, t->first->posi);
			t->second->normal.normalise();
			t->last->normal += getNormal(t->last->posi, t->first->posi, t->second->posi);
			t->last->normal.normalise();
		}
	}


	StarWars(){
		FrameListenerTurrets01 = NULL;
		FrameListenerNave01 = NULL;
		FrameListenerHelices01 = NULL;
	}

	~StarWars(){
		if(FrameListenerTurrets01){
			delete FrameListenerTurrets01;
		}
		if(FrameListenerNave01){
			delete FrameListenerNave01;
		}
		if(FrameListenerHelices01){
			delete FrameListenerHelices01;
		}
	}

	void createFrameListener(){
		FrameListenerTurrets01 = new FrameListenerTurrets(nave, turrets);
		FrameListenerNave01 = new FrameListenerNave(nave, mWindow, camera_node);
		FrameListenerHelices01 = new FrameListenerHelices(helices);
		mRoot->addFrameListener(FrameListenerTurrets01);
		mRoot->addFrameListener(FrameListenerNave01);
		mRoot->addFrameListener(FrameListenerHelices01);
	}

	void createCamera() {

		mCamera = mSceneMgr->createCamera("MyCamera1");
		mCamera->setPosition(0, 150, 900);
		mCamera->lookAt(0.0f, 0.0f, 0.0f);
		mCamera->setNearClipDistance(5);

	}

	void createScene()
	{
		
		mSceneMgr->setAmbientLight(Ogre::ColourValue(1.0, 1.0, 1.0));
		mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
		mSceneMgr->setSkyBox(true, "MySkyBox1");

		Ogre::Light* light = mSceneMgr->createLight("Light01");
		light->setType(Ogre::Light::LT_POINT);
		light->setPosition(1670.0f, 460.0f, -15300.0f);
		light->setDiffuseColour(0.0f, 1.0f, 1.0f);
		light->setSpecularColour(0.0f, 1.0f, 1.0f);

		Ogre::Entity* entEscenario01 = mSceneMgr->createEntity("EscenarioBase01","proyectoOgreI.mesh");
		Ogre::SceneNode* nodeEscenario01 = mSceneMgr->createSceneNode("NodeMesh01");		
		mSceneMgr->getRootSceneNode()->addChild(nodeEscenario01);
		nodeEscenario01->attachObject(entEscenario01);
		entEscenario01->setCastShadows(true);
		camera_node = mSceneMgr->createSceneNode("Camera");
		camera_node->attachObject(mCamera);
		mSceneMgr->getRootSceneNode()->addChild(camera_node);


		// Primera torreta
		turrets[0] = mSceneMgr->createSceneNode("Turret0");
		mSceneMgr->getRootSceneNode()->addChild(turrets[0]);
		turrets[0]->setPosition(-1670.0f, -50.0f, -8600.0f);

		Ogre::Entity* cil0_ent = mSceneMgr->createEntity("Cilindro0", "usb_cilindro02.mesh");
		Ogre::SceneNode* cil0_node = turrets[0]->createChildSceneNode("Cilindro0");
		cil0_ent->setMaterialName("lambert3");
		cil0_ent->setCastShadows(true);
		cil0_node->attachObject(cil0_ent);
		cil0_node->scale(86.0f, 50.0f, 86.0f);

		Ogre::Entity* hel0_ent = mSceneMgr->createEntity("Helix0", "usb_helix.mesh");
		Ogre::SceneNode* hel0_node = turrets[0]->createChildSceneNode("Helix0");
		hel0_ent->setMaterialName("lambert3");
		hel0_node->attachObject(hel0_ent);
		hel0_node->scale(10.0f, 16.0f, 10.0f);
		hel0_node->translate(0.0f, 360.0f, 0.0f);

		Ogre::Entity* cilm0_ent = mSceneMgr->createEntity("CilindroMini0", "usb_cilindro.mesh");
		Ogre::SceneNode* cilm0_node = turrets[0]->createChildSceneNode("CilindroMini0");
		cilm0_ent->setMaterialName("lambert3");
		cilm0_node->attachObject(cilm0_ent);
		cilm0_node->scale(3.0f, 16.0f, 3.0f);
		cilm0_node->translate(0.0f, 360.0f, 0.0f);

		Ogre::Entity* pla0_ent = mSceneMgr->createEntity("Platillo0", "usb_formacurva.mesh");
		Ogre::SceneNode* pla0_node = turrets[0]->createChildSceneNode("Platillo0");
		pla0_ent->setMaterialName("lambert3");
		pla0_node->attachObject(pla0_ent);
		pla0_node->scale(10.0f, 10.0f, 10.0f);
		pla0_node->translate(0.0f, 460.0f, 00.0f);
		
		Ogre::Entity* ded0_ent = mSceneMgr->createEntity("Dedocaedro0", "usb_dodecaedro.mesh");
		Ogre::SceneNode* ded0_node = pla0_node->createChildSceneNode("Dedocaedro0");
		ded0_ent->setMaterialName("lambert3");
		ded0_node->attachObject(ded0_ent);
		ded0_node->translate(15.0f, 10.0f, 0.0f);

		// Segunda torreta
		turrets[1] = mSceneMgr->createSceneNode("Turret1");
		mSceneMgr->getRootSceneNode()->addChild(turrets[1]);
		turrets[1]->setPosition(1670.0f, -50.0f, -15300.0f);

		Ogre::Entity* cil1_ent = mSceneMgr->createEntity("Cilindro1", "usb_cilindro02.mesh");
		Ogre::SceneNode* cil1_node = turrets[1]->createChildSceneNode("Cilindro1");
		cil1_ent->setMaterialName("lambert3");
		cil1_node->attachObject(cil1_ent);
		cil1_node->scale(86.0f, 50.0f, 86.0f);

		Ogre::Entity* hel1_ent = mSceneMgr->createEntity("Helix1", "usb_helix.mesh");
		Ogre::SceneNode* hel1_node = turrets[1]->createChildSceneNode("Helix1");
		hel1_ent->setMaterialName("lambert3");
		hel1_node->attachObject(hel1_ent);
		hel1_node->scale(10.0f, 16.0f, 10.0f);
		hel1_node->translate(0.0f, 360.0f, 0.0f);

		Ogre::Entity* cilm1_ent = mSceneMgr->createEntity("CilindroMini1", "usb_cilindro.mesh");
		Ogre::SceneNode* cilm1_node = turrets[1]->createChildSceneNode("CilindroMini1");
		cilm1_ent->setMaterialName("lambert3");
		cilm1_node->attachObject(cilm1_ent);
		cilm1_node->scale(3.0f, 16.0f, 3.0f);
		cilm1_node->translate(0.0f, 360.0f, 0.0f);

		Ogre::Entity* pla1_ent = mSceneMgr->createEntity("Platillo1", "usb_formacurva.mesh");
		Ogre::SceneNode* pla1_node = turrets[1]->createChildSceneNode("Platillo1");
		pla1_ent->setMaterialName("lambert3");
		pla1_node->attachObject(pla1_ent);
		pla1_node->scale(10.0f, 10.0f, 10.0f);
		pla1_node->translate(0.0f, 460.0f, 00.0f);
		
		Ogre::Entity* ded1_ent = mSceneMgr->createEntity("Dedocaedro1", "usb_dodecaedro.mesh");
		Ogre::SceneNode* ded1_node = pla1_node->createChildSceneNode("Dedocaedro1");
		ded1_ent->setMaterialName("lambert3");
		ded1_node->attachObject(ded1_ent);
		ded1_node->translate(-15.0f, 10.0f, 0.0f);
		
		//
		// Tercera torreta
		//
		turrets[2] = mSceneMgr->createSceneNode("Turret2");
		mSceneMgr->getRootSceneNode()->addChild(turrets[2]);
		turrets[2]->setPosition(8400.0f, -50.0f, -23500.0f);

		Ogre::Entity* cil2_ent = mSceneMgr->createEntity("Cilindro2", "usb_cilindro02.mesh");
		Ogre::SceneNode* cil2_node = turrets[2]->createChildSceneNode("Cilindro2");
		cil2_ent->setMaterialName("lambert3");
		cil2_node->attachObject(cil2_ent);
		cil2_node->scale(86.0f, 50.0f, 86.0f);

		Ogre::Entity* hel2_ent = mSceneMgr->createEntity("Helix2", "usb_helix.mesh");
		Ogre::SceneNode* hel2_node = turrets[2]->createChildSceneNode("Helix2");
		hel2_ent->setMaterialName("lambert3");
		hel2_node->attachObject(hel2_ent);
		hel2_node->scale(10.0f, 16.0f, 10.0f);
		hel2_node->translate(0.0f, 360.0f, 0.0f);

		Ogre::Entity* cilm2_ent = mSceneMgr->createEntity("CilindroMini2", "usb_cilindro.mesh");
		Ogre::SceneNode* cilm2_node = turrets[2]->createChildSceneNode("CilindroMini2");
		cilm2_ent->setMaterialName("lambert3");
		cilm2_node->attachObject(cilm2_ent);
		cilm2_node->scale(3.0f, 16.0f, 3.0f);
		cilm2_node->translate(0.0f, 360.0f, 0.0f);

		Ogre::Entity* pla2_ent = mSceneMgr->createEntity("Platillo2", "usb_formacurva.mesh");
		Ogre::SceneNode* pla2_node = turrets[2]->createChildSceneNode("Platillo2");
		pla2_ent->setMaterialName("lambert3");
		pla2_node->attachObject(pla2_ent);
		pla2_node->scale(10.0f, 10.0f, 10.0f);
		pla2_node->translate(0.0f, 460.0f, 00.0f);
		
		Ogre::Entity* ded2_ent = mSceneMgr->createEntity("Dedocaedro2", "usb_dodecaedro.mesh");
		Ogre::SceneNode* ded2_node = pla2_node->createChildSceneNode("Dedocaedro2");
		ded2_ent->setMaterialName("lambert3");
		ded2_node->attachObject(ded2_ent);
		ded2_node->translate(15.0f, 10.0f, 0.0f);
		
		
		//
		// Cuarta torreta
		//
		turrets[3] = mSceneMgr->createSceneNode("Turret3");
		mSceneMgr->getRootSceneNode()->addChild(turrets[3]);
		turrets[3]->setPosition(24000.0f, -50.0f, -23400.0f);

		Ogre::Entity* cil3_ent = mSceneMgr->createEntity("Cilindro3", "usb_cilindro02.mesh");
		Ogre::SceneNode* cil3_node = turrets[3]->createChildSceneNode("Cilindro3");
		cil3_ent->setMaterialName("lambert3");
		cil3_node->attachObject(cil3_ent);
		cil3_node->scale(86.0f, 50.0f, 86.0f);

		Ogre::Entity* hel3_ent = mSceneMgr->createEntity("Helix3", "usb_helix.mesh");
		Ogre::SceneNode* hel3_node = turrets[3]->createChildSceneNode("Helix3");
		hel3_ent->setMaterialName("lambert3");
		hel3_node->attachObject(hel3_ent);
		hel3_node->scale(10.0f, 16.0f, 10.0f);
		hel3_node->translate(0.0f, 360.0f, 0.0f);

		Ogre::Entity* cilm3_ent = mSceneMgr->createEntity("CilindroMini3", "usb_cilindro.mesh");
		Ogre::SceneNode* cilm3_node = turrets[3]->createChildSceneNode("CilindroMini3");
		cilm3_ent->setMaterialName("lambert3");
		cilm3_node->attachObject(cilm3_ent);
		cilm3_node->scale(3.0f, 16.0f, 3.0f);
		cilm3_node->translate(0.0f, 360.0f, 0.0f);

		Ogre::Entity* pla3_ent = mSceneMgr->createEntity("Platillo3", "usb_formacurva.mesh");
		Ogre::SceneNode* pla3_node = turrets[3]->createChildSceneNode("Platillo3");
		pla3_ent->setMaterialName("lambert3");
		pla3_node->attachObject(pla3_ent);
		pla3_node->scale(10.0f, 10.0f, 10.0f);
		pla3_node->translate(0.0f, 460.0f, 00.0f);
		
		Ogre::Entity* ded3_ent = mSceneMgr->createEntity("Dedocaedro3", "usb_dodecaedro.mesh");
		Ogre::SceneNode* ded3_node = pla3_node->createChildSceneNode("Dedocaedro3");
		ded3_ent->setMaterialName("lambert3");
		ded3_node->attachObject(ded3_ent);
		ded3_node->translate(15.0f, 10.0f, 0.0f);
		
		//
		// Quinta torreta
		//
		turrets[4] = mSceneMgr->createSceneNode("Turret4");
		mSceneMgr->getRootSceneNode()->addChild(turrets[4]);
		turrets[4]->setPosition(15500.0f, -50.0f, -18900.0f);

		Ogre::Entity* cil4_ent = mSceneMgr->createEntity("Cilindro4", "usb_cilindro02.mesh");
		Ogre::SceneNode* cil4_node = turrets[4]->createChildSceneNode("Cilindro4");
		cil4_ent->setMaterialName("lambert3");
		cil4_ent->setCastShadows(true);
		cil4_node->attachObject(cil4_ent);
		cil4_node->scale(86.0f, 50.0f, 86.0f);

		Ogre::Entity* hel4_ent = mSceneMgr->createEntity("Helix4", "usb_helix.mesh");
		Ogre::SceneNode* hel4_node = turrets[4]->createChildSceneNode("Helix4");
		hel4_ent->setMaterialName("lambert3");
		hel4_node->attachObject(hel4_ent);
		hel4_node->scale(10.0f, 16.0f, 10.0f);
		hel4_node->translate(0.0f, 360.0f, 0.0f);

		Ogre::Entity* cilm4_ent = mSceneMgr->createEntity("CilindroMini4", "usb_cilindro.mesh");
		Ogre::SceneNode* cilm4_node = turrets[4]->createChildSceneNode("CilindroMini4");
		cilm4_ent->setMaterialName("lambert3");
		cilm4_node->attachObject(cilm4_ent);
		cilm4_node->scale(3.0f, 16.0f, 3.0f);
		cilm4_node->translate(0.0f, 360.0f, 0.0f);

		Ogre::Entity* pla4_ent = mSceneMgr->createEntity("Platillo4", "usb_formacurva.mesh");
		Ogre::SceneNode* pla4_node = turrets[4]->createChildSceneNode("Platillo4");
		pla4_ent->setMaterialName("lambert3");
		pla4_node->attachObject(pla4_ent);
		pla4_node->scale(10.0f, 10.0f, 10.0f);
		pla4_node->translate(0.0f, 460.0f, 00.0f);
		
		Ogre::Entity* ded4_ent = mSceneMgr->createEntity("Dedocaedro4", "usb_dodecaedro.mesh");
		Ogre::SceneNode* ded4_node = pla4_node->createChildSceneNode("Dedocaedro4");
		ded4_ent->setMaterialName("lambert3");
		ded4_node->attachObject(ded4_ent);
		ded4_node->translate(15.0f, 10.0f, 0.0f);
				
		//
		// Sexta torreta
		//
		turrets[5] = mSceneMgr->createSceneNode("Turret5");
		mSceneMgr->getRootSceneNode()->addChild(turrets[5]);
		turrets[5]->setPosition(-10000.0f, -50.0f, -23500.0f);

		Ogre::Entity* cil5_ent = mSceneMgr->createEntity("Cilindro5", "usb_cilindro02.mesh");
		Ogre::SceneNode* cil5_node = turrets[5]->createChildSceneNode("Cilindro5");
		cil5_ent->setMaterialName("lambert3");
		cil5_node->attachObject(cil5_ent);
		cil5_node->scale(86.0f, 50.0f, 86.0f);

		Ogre::Entity* hel5_ent = mSceneMgr->createEntity("Helix5", "usb_helix.mesh");
		Ogre::SceneNode* hel5_node = turrets[5]->createChildSceneNode("Helix5");
		hel5_ent->setMaterialName("lambert3");
		hel5_node->attachObject(hel5_ent);
		hel5_node->scale(10.0f, 16.0f, 10.0f);
		hel5_node->translate(0.0f, 360.0f, 0.0f);

		Ogre::Entity* cilm5_ent = mSceneMgr->createEntity("CilindroMini5", "usb_cilindro.mesh");
		Ogre::SceneNode* cilm5_node = turrets[5]->createChildSceneNode("CilindroMini5");
		cilm5_ent->setMaterialName("lambert3");
		cilm5_node->attachObject(cilm5_ent);
		cilm5_node->scale(3.0f, 16.0f, 3.0f);
		cilm5_node->translate(0.0f, 360.0f, 0.0f);

		Ogre::Entity* pla5_ent = mSceneMgr->createEntity("Platillo5", "usb_formacurva.mesh");
		Ogre::SceneNode* pla5_node = turrets[5]->createChildSceneNode("Platillo5");
		pla5_ent->setMaterialName("lambert3");
		pla5_node->attachObject(pla5_ent);
		pla5_node->scale(10.0f, 10.0f, 10.0f);
		pla5_node->translate(0.0f, 460.0f, 00.0f);
		
		Ogre::Entity* ded5_ent = mSceneMgr->createEntity("Dedocaedro5", "usb_dodecaedro.mesh");
		Ogre::SceneNode* ded5_node = pla5_node->createChildSceneNode("Dedocaedro5");
		ded5_ent->setMaterialName("lambert3");
		ded5_node->attachObject(ded5_ent);
		ded5_node->translate(15.0f, 10.0f, 0.0f);
		
		
		//
		// Septima torreta
		//
		turrets[6] = mSceneMgr->createSceneNode("Turret6");
		mSceneMgr->getRootSceneNode()->addChild(turrets[6]);
		turrets[6]->setPosition(-23100.0f, -50.0f, -23400.0f);

		Ogre::Entity* cil6_ent = mSceneMgr->createEntity("Cilindro6", "usb_cilindro02.mesh");
		Ogre::SceneNode* cil6_node = turrets[6]->createChildSceneNode("Cilindro6");
		cil6_ent->setMaterialName("lambert3");
		cil6_node->attachObject(cil6_ent);
		cil6_node->scale(86.0f, 50.0f, 86.0f);

		Ogre::Entity* hel6_ent = mSceneMgr->createEntity("Helix6", "usb_helix.mesh");
		Ogre::SceneNode* hel6_node = turrets[6]->createChildSceneNode("Helix6");
		hel6_ent->setMaterialName("lambert3");
		hel6_node->attachObject(hel6_ent);
		hel6_node->scale(10.0f, 16.0f, 10.0f);
		hel6_node->translate(0.0f, 360.0f, 0.0f);

		Ogre::Entity* cilm6_ent = mSceneMgr->createEntity("CilindroMini6", "usb_cilindro.mesh");
		Ogre::SceneNode* cilm6_node = turrets[6]->createChildSceneNode("CilindroMini6");
		cilm6_ent->setMaterialName("lambert3");
		cilm6_node->attachObject(cilm6_ent);
		cilm6_node->scale(3.0f, 16.0f, 3.0f);
		cilm6_node->translate(0.0f, 360.0f, 0.0f);

		Ogre::Entity* pla6_ent = mSceneMgr->createEntity("Platillo6", "usb_formacurva.mesh");
		Ogre::SceneNode* pla6_node = turrets[6]->createChildSceneNode("Platillo6");
		pla6_ent->setMaterialName("lambert3");
		pla6_node->attachObject(pla6_ent);
		pla6_node->scale(10.0f, 10.0f, 10.0f);
		pla6_node->translate(0.0f, 460.0f, 00.0f);
		
		Ogre::Entity* ded6_ent = mSceneMgr->createEntity("Dedocaedro6", "usb_dodecaedro.mesh");
		Ogre::SceneNode* ded6_node = pla6_node->createChildSceneNode("Dedocaedro6");
		ded6_ent->setMaterialName("lambert3");
		ded6_node->attachObject(ded6_ent);
		ded6_node->translate(15.0f, 10.0f, 0.0f);
		
		//
		// Octava torreta
		//
		turrets[7] = mSceneMgr->createSceneNode("Turret7");
		mSceneMgr->getRootSceneNode()->addChild(turrets[7]);
		turrets[7]->setPosition(-17100.0f, -50.0f, -18900.0f);

		Ogre::Entity* cil7_ent = mSceneMgr->createEntity("Cilindro7", "usb_cilindro02.mesh");
		Ogre::SceneNode* cil7_node = turrets[7]->createChildSceneNode("Cilindro7");
		cil7_ent->setMaterialName("lambert3");
		cil7_node->attachObject(cil7_ent);
		cil7_node->scale(86.0f, 50.0f, 86.0f);

		Ogre::Entity* hel7_ent = mSceneMgr->createEntity("Helix7", "usb_helix.mesh");
		Ogre::SceneNode* hel7_node = turrets[7]->createChildSceneNode("Helix7");
		hel7_ent->setMaterialName("lambert3");
		hel7_node->attachObject(hel7_ent);
		hel7_node->scale(10.0f, 16.0f, 10.0f);
		hel7_node->translate(0.0f, 360.0f, 0.0f);

		Ogre::Entity* cilm7_ent = mSceneMgr->createEntity("CilindroMini7", "usb_cilindro.mesh");
		Ogre::SceneNode* cilm7_node = turrets[7]->createChildSceneNode("CilindroMini7");
		cilm7_ent->setMaterialName("lambert3");
		cilm7_node->attachObject(cilm7_ent);
		cilm7_node->scale(3.0f, 16.0f, 3.0f);
		cilm7_node->translate(0.0f, 360.0f, 0.0f);

		Ogre::Entity* pla7_ent = mSceneMgr->createEntity("Platillo7", "usb_formacurva.mesh");
		Ogre::SceneNode* pla7_node = turrets[7]->createChildSceneNode("Platillo7");
		pla7_ent->setMaterialName("lambert3");
		pla7_node->attachObject(pla7_ent);
		pla7_node->scale(10.0f, 10.0f, 10.0f);
		pla7_node->translate(0.0f, 460.0f, 00.0f);
		
		Ogre::Entity* ded7_ent = mSceneMgr->createEntity("Dedocaedro7", "usb_dodecaedro.mesh");
		Ogre::SceneNode* ded7_node = pla7_node->createChildSceneNode("Dedocaedro7");
		ded7_ent->setMaterialName("lambert3");
		ded7_node->attachObject(ded7_ent);
		ded7_node->translate(15.0f, 10.0f, 0.0f);


		nave = camera_node->createChildSceneNode("Nave");
		Ogre::Entity* entBase = mSceneMgr->createEntity("TieBase", "sphere.mesh");
		entBase->setMaterialName("lambert3");
		nave->scale(0.9,1.0,1.0);
		nave->yaw(Ogre::Degree(90.0f));
		nave->translate(0.0,-100.0,0.0);
		nave->attachObject(entBase);

		

		Ogre::SceneNode* nodeProtub = mSceneMgr->createSceneNode("NodeProtub");
		Ogre::Entity* entProtub = mSceneMgr->createEntity("TieWindow", "usb_torus.mesh");
		entProtub->setMaterialName("lambert3");
		nave->addChild(nodeProtub);
		
		nodeProtub->roll(Ogre::Degree(90.0f));
		nodeProtub->scale(6.5,3.0,6.5);
		nodeProtub->translate(100.0,0.0,0.0);
		nodeProtub->attachObject(entProtub);

		//Hélice
		Ogre::Entity* entHelice1 = mSceneMgr->createEntity("Escen", "column.mesh");
		entHelice1->setMaterialName("lambert3");
		helices[0] = mSceneMgr->createSceneNode("NodeHelice");
		Ogre::SceneNode* nodeCentro = mSceneMgr->createSceneNode("NodeCentro");
		mSceneMgr->getRootSceneNode()->addChild(helices[0]);
		helices[0]->addChild(nodeCentro);
		helices[0]->setPosition(-13500.0f, 0.0f, -18600.0f);
		helices[0]->scale(3.0,1.5,3.0);
		helices[0]->roll(Ogre::Degree(90.0f));
		//ROTAR CON YAW AQUI, a helices[0]
		nodeCentro->scale(10.0, 0.5,10.0);
		nodeCentro->attachObject(entHelice1);

		//Aspa1
		Ogre::Entity* entAspa1 = mSceneMgr->createEntity("Asp1", "usb_prisma.mesh");
		entAspa1->setMaterialName("lambert3");
		Ogre::SceneNode* nodeAspa1 = mSceneMgr->createSceneNode("NodeAsp1");
		helices[0]->addChild(nodeAspa1);
		nodeAspa1->scale(350.0, 60.0,150.0);
		nodeAspa1->translate(800.0, 0.0,0.0);
		nodeAspa1->yaw(Ogre::Degree(190.f));
		nodeAspa1->attachObject(entAspa1);

		//Aspa2
		Ogre::Entity* entAspa2 = mSceneMgr->createEntity("Asp2", "usb_prisma.mesh");
		entAspa2->setMaterialName("lambert3");
		Ogre::SceneNode* nodeAspa2 = mSceneMgr->createSceneNode("NodeAsp2");
		helices[0]->addChild(nodeAspa2);
		nodeAspa2->scale(350.0, 60.0,150.0);
		nodeAspa2->translate(0.0, 0.0,-800.0);
		nodeAspa2->yaw(Ogre::Degree(-90.0f));
		nodeAspa2->attachObject(entAspa2);

		//Aspa3
		Ogre::Entity* entAspa3 = mSceneMgr->createEntity("Asp3", "usb_prisma.mesh");
		entAspa3->setMaterialName("lambert3");
		Ogre::SceneNode* nodeAspa3 = mSceneMgr->createSceneNode("NodeAsp3");
		helices[0]->addChild(nodeAspa3);
		nodeAspa3->scale(350.0, 60.0,150.0);
		nodeAspa3->translate(0.0, 0.0, 800.0);
		nodeAspa3->yaw(Ogre::Degree(90.0f));
		nodeAspa3->attachObject(entAspa3);

		//Aspa4
		Ogre::Entity* entAspa4 = mSceneMgr->createEntity("Asp4", "usb_prisma.mesh");
		entAspa4->setMaterialName("lambert3");
		Ogre::SceneNode* nodeAspa4 = mSceneMgr->createSceneNode("NodeAsp4");
		helices[0]->addChild(nodeAspa4);
		nodeAspa4->scale(350.0, 60.0,150.0);
		nodeAspa4->translate(-800.0, 0.0, 0.0);
		//nodeAspa4->yaw(Ogre::Degree(90.0f));
		nodeAspa4->attachObject(entAspa4);

		//Hélice2
		Ogre::Entity* entHelice2 = mSceneMgr->createEntity("Escen2", "column.mesh");
		entHelice2->setMaterialName("lambert3");
		helices[1] = mSceneMgr->createSceneNode("NodeHelice2");
		Ogre::SceneNode* nodeCentro2 = mSceneMgr->createSceneNode("NodeCentro2");
		mSceneMgr->getRootSceneNode()->addChild(helices[1]);
		helices[1]->addChild(nodeCentro2);
		helices[1]->setPosition(16500.0f, 0.0f, -18600.0f);
		helices[1]->scale(3.0,1.5,3.0);
		helices[1]->roll(Ogre::Degree(90.0f));
		//ROTAR CON YAW AQUI, a helices[0]
		nodeCentro2->scale(10.0, 0.5,10.0);
		nodeCentro2->attachObject(entHelice2);

		//Aspa1
		Ogre::Entity* entAspa12 = mSceneMgr->createEntity("Asp12", "usb_prisma.mesh");
		entAspa12->setMaterialName("lambert3");
		Ogre::SceneNode* nodeAspa12 = mSceneMgr->createSceneNode("NodeAsp12");
		helices[1]->addChild(nodeAspa12);
		nodeAspa12->scale(350.0, 60.0,150.0);
		nodeAspa12->translate(800.0, 0.0,0.0);
		nodeAspa12->yaw(Ogre::Degree(190.f));
		nodeAspa12->attachObject(entAspa12);

		//Aspa2
		Ogre::Entity* entAspa22 = mSceneMgr->createEntity("Asp22", "usb_prisma.mesh");
		entAspa22->setMaterialName("lambert3");
		Ogre::SceneNode* nodeAspa22 = mSceneMgr->createSceneNode("NodeAsp22");
		helices[1]->addChild(nodeAspa22);
		nodeAspa22->scale(350.0, 60.0,150.0);
		nodeAspa22->translate(0.0, 0.0,-800.0);
		nodeAspa22->yaw(Ogre::Degree(-90.0f));
		nodeAspa22->attachObject(entAspa22);

		//Aspa3
		Ogre::Entity* entAspa32 = mSceneMgr->createEntity("Asp32", "usb_prisma.mesh");
		entAspa32->setMaterialName("lambert3");
		Ogre::SceneNode* nodeAspa32 = mSceneMgr->createSceneNode("NodeAsp32");
		helices[1]->addChild(nodeAspa32);
		nodeAspa32->scale(350.0, 60.0,150.0);
		nodeAspa32->translate(0.0, 0.0, 800.0);
		nodeAspa32->yaw(Ogre::Degree(90.0f));
		nodeAspa32->attachObject(entAspa32);

		//Aspa4
		Ogre::Entity* entAspa42 = mSceneMgr->createEntity("Asp42", "usb_prisma.mesh");
		entAspa42->setMaterialName("lambert3");
		Ogre::SceneNode* nodeAspa42 = mSceneMgr->createSceneNode("NodeAsp42");
		helices[1]->addChild(nodeAspa42);
		nodeAspa42->scale(350.0, 60.0,150.0);
		nodeAspa42->translate(-800.0, 0.0, 0.0);
		//nodeAspa4->yaw(Ogre::Degree(90.0f));
		nodeAspa42->attachObject(entAspa42);

		//Modelo manual nave
		Ogre::SceneNode* sceneManualBase1 = mSceneMgr->createSceneNode("NodeManual");
		nave->addChild(sceneManualBase1);
		sceneManualBase1->translate(0.0,0.0,-94.0);
		ManualObject* manual = mSceneMgr->createManualObject("manual");
		manual->begin("lambert3", RenderOperation::OT_TRIANGLE_LIST);

		calcularNormales();

		Ogre::Vector3 norm;
		for(int i = 0; i < cantTriangulo; i++) {
			Vertex* temp1 = triangulos.at(i)->first;
			Vertex* temp2 = triangulos.at(i)->second;
			Vertex* temp3 = triangulos.at(i)->last;

			manual->position(temp1->posi.x, temp1->posi.y, temp1->posi.z);
			manual->normal(temp1->normal);

			manual->position(temp2->posi.x, temp2->posi.y, temp2->posi.z);
			manual->normal(temp2->normal);

			manual->position(temp3->posi.x, temp3->posi.y, temp3->posi.z);
			manual->normal(temp3->normal);
		}
		
 
		manual->end();
		sceneManualBase1->attachObject(manual);

		//Drawing manual2
		Ogre::SceneNode* sceneManualBase2 = mSceneMgr->createSceneNode("NodeManual2");
		nave->addChild(sceneManualBase2);
		sceneManualBase2->translate(0.0,0.0,94.0);
		sceneManualBase2->pitch(Ogre::Degree(180.0f));
		ManualObject* manual2 = mSceneMgr->createManualObject("manual2");
		manual2->begin("lambert3", RenderOperation::OT_TRIANGLE_LIST);
 
		for(int i = 0; i < cantTriangulo; i++) {
			Vertex* temp1 = triangulos.at(i)->first;
			Vertex* temp2 = triangulos.at(i)->second;
			Vertex* temp3 = triangulos.at(i)->last;

			manual2->position(temp1->posi.x, temp1->posi.y, temp1->posi.z);
			manual2->normal(temp1->normal);

			manual2->position(temp2->posi.x, temp2->posi.y, temp2->posi.z);
			manual2->normal(temp2->normal);

			manual2->position(temp3->posi.x, temp3->posi.y, temp3->posi.z);
			manual2->normal(temp3->normal);
		}
 
		manual2->end();
		sceneManualBase2->attachObject(manual2);
	}

};


int main (void)
{

	StarWars app;
	app.go();
	return 0;
}