import { useState, useEffect } from 'react'
import { Form, InputNumber, Button, message, Layout } from 'antd'
import './App.css'

const { Header, Content, Footer } = Layout

function App() {
  const [form] = Form.useForm()
  const [loading, setLoading] = useState(false)
  const [deviceState, setDeviceState] = useState('idle')

useEffect(() => {
  const id = setInterval(async () => {
    try {
      const res = await fetch('http://10.21.245.247/status')
      const data = await res.json()
      setDeviceState(data.state)
    } catch (err) {
      setDeviceState('offline')
    }
  }, 1000)

  return () => clearInterval(id)
}, [])

  const onFinish = async (values: { pace: any; distance: any }) => {
    try {
      setLoading(true)

      const payload = {
        pace: values.pace,
        distance: values.distance,
      }

      const res = await fetch('http://10.21.245.247/set', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify(payload),
      })

      if (!res.ok) {
        throw new Error(`Request failed: ${res.status}`)
      }

      message.success('Values sent to ESP32')
      form.resetFields()
    } catch (err) {
      message.error('Failed to send values')
      console.error(err)
    } finally {
      setLoading(false)
    }
  }

  return (
    <Layout>
      <Header style={{height:128}}>
        <h1 style={{ color: 'white', textAlign: 'left' }}>Rabbit Lights</h1>
      </Header>
      <Content>
        <section id="center">
          <div>
            <h1 style={{color: 'black'}}>Set Lights</h1>

            <Form form={form} onFinish={onFinish} layout="vertical" disabled={deviceState === 'running '}>
              <Form.Item
                label="Pace (seconds per mile)"
                name="pace"
                rules={[{ required: true, message: 'Enter a pace value' }]}
              >
                <InputNumber style={{ width: '100%' }} />
              </Form.Item>

              <Form.Item
                label="Distance (meters)"
                name="distance"
                rules={[{ required: true, message: 'Enter a distance value' }]}
              >
                <InputNumber style={{ width: '100%' }} />
              </Form.Item>

              <Form.Item>
                <Button type="primary" htmlType="submit" loading={loading}>
                  Submit
                </Button>
              </Form.Item>
            </Form>
          </div>
        </section>
      </Content>
      <Footer>
        <p style={{ textAlign: 'left' }}>Device State: {deviceState}</p>
      </Footer>
    </Layout>
  )
}

export default App